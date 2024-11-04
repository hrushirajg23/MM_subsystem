
#include"fs.h"
#include"mm.h"
#include<stdlib.h>
#include<include/asm/system.h>

                //640KB             1024KB i.e 1MB
#if (BUFFER_END > 0xA0000 && BUFFER_END <=0x100000)
#error "bad buffer value"
#endif
int end=0x200000;
int NR_BUFFERS=0;
struct buffer_head* start_buffer=(struct buffer_head*)&end;
struct buffer_head* hash_table[NR_HASH];
static struct buffer_head* freelist;
static struct task* buffer_wait=NULL;

#define _hashfn(dev,block) (((unsigned)(dev^block))%NR_HASH)
#define hash(dev,block) hash_table[_hashfn(dev,block)]


#define REMOVE_FREE 2
#define REMOVE_QUEUE 1
#define REMOVE 3

#define INSERT_FREE 2
#define INSERT_QUEUE 1
#define INSERT 3

/*
    @attention:
    1.The kernel frequently uses "delayed write" to avoid unnecessary disk writes
    ,leaving the block in buffer cache and hoping for a cache hit on block.
    More the buffers, more the cache hit chances
    2. Maintain file system integrity as a single image of disk blocks
        avoid data corruption
    3. high throughput
*/
void buffer_init(void){
    struct buffer_head* h=start_buffer;/*
        stores the actual starting buffer_head address 

            whereas BUFFER_END stores the max address where buffer data will reside
            . IT will come down from tht mark
    
    */

   /*
      b=2mb  -----------------|
            |1024,1024,1024
            |...coming down
            |
            |will collide at this point and loop will stop,
            |look at test.c
            |               
            |going up...  
            |36,36,36
        h =start buffer  
   
    
   */
    void* b=(void*)BUFFER_END;
    int i=0;
    /*started from h+1 has it needed a prev_free dummy buffer
    i.e start_buffer */
    while((b-=BLOCK_SIZE)>=(void*)(h+1)){
        h->b_dev=0;
        h->b_data=(char*)b;
        h->b_dirt=false;
        h->count=0;
        h->b_lock=false;
        h->b_uptodate=false;
        h->b_prev=NULL;
        h->b_next=NULL;
        h->b_wait=NULL;
        h->b_next_free=h+1;
        h->b_prev_free=h-1;
        h++; /*h+=sizeof(buffer)*/
        NR_BUFFERS++;
    }
    h--;
    freelist=start_buffer;
    freelist->b_prev_free=h;
    h->b_next_free=freelist;
    for(i=0;i<NR_HASH;i++)
        hash_table[i]=NULL;
    
}



inline void wait_on_buffer(struct buffer_head* bh){
    cli();
        while(bh->b_lock)
            sleep_on(&bh->b_wait);
    sti();
}


int sys_sync(void){
    int i=0;
    struct buffer_head* bh=NULL;
    
    sync_inodes(); /*write file data into buffers first*/
    
    bh=start_buffer;
    for(i=0;i<NR_BUFFERS;i++){
        wait_on_buffer(bh);
        if(bh->b_dirt)
            ll_rw_block(WRITE,bh);
    }
    return 0;
}


/*


    @attention:
    freelist uses doubly-circular list ,whereas
    hashqueues use doubly linked list

*/

static void inline insert_into_queues(struct buffer_head* bh,u_int8_t status){
    /*
        put at end of free list
    */
   if(status==INSERT || status == INSERT_FREE){
     bh->b_next_free=freelist;
    bh->b_prev_free=freelist->b_prev_free;
    
    /*freelist chya tail chya pudhe adding new bh*/
    freelist->b_prev_free->b_next_free=bh;
    freelist->b_prev_free=bh;

   }

    /*
        putting in hashqueue
    */
   if(status==INSERT || status==INSERT_QUEUE){
    bh->b_next=NULL;
   bh->b_prev=NULL;
   if(!bh->b_dev)return; /*no device associated with it*/

    bh->b_next=hash(bh->b_dev,bh->b_blockno); /*
        ataching it to head of appropriate queue
    */
    hash(bh->b_dev,bh->b_blockno)=bh;
    if(bh->b_next!=NULL){
        bh->b_next->b_prev=bh;
    }
   }
}   


static inline void remove_from_queues(struct buffer_head* bh,u_int8_t status){


/*removing from hash-queue*/

  if(status==REMOVE || status==REMOVE_QUEUE){
      if(bh->b_next)
        bh->b_next->b_prev=bh->b_prev;
    if(bh->b_prev)
        bh->b_prev->b_next=bh->b_next;
    if(hash(bh->b_dev,bh->b_blockno)==bh) /*only single  or the first element*/
           bh->b_next->b_prev=bh->b_next;    
  }

/*
    remove from free list
*/    
/*
    I've kept the status since sometimes you just need to remove from 
*/
   if(status==REMOVE_FREE || status ==REMOVE){
     if((bh->b_prev_free==NULL) || (bh->b_next_free=NULL)){
        printf("freelist sucks\n");
    }

    bh->b_prev_free->b_next_free=bh->b_next_free;
    bh->b_next_free->b_prev_free=bh->b_prev_free;
    if(freelist==bh)
        freelist=bh->b_next;
   }

}

static  struct buffer_head* find_buffer(int dev,int block){
    struct buffer_head* temp=NULL;
    for(temp=hash(dev,block);temp!=NULL;temp++){
            if(temp->b_dev==dev && temp->b_blockno==block)
                return temp;
    }
    return NULL;
}


struct buffer_head* get_b_from_htable(int dev,int block){
    struct buffer_head* bh=NULL;
    loop:
        if((bh=find_buffer(dev,block))==NULL)
            return NULL;
        bh->count++;
        wait_on_buffer(bh);
        if(bh->b_dev!=dev || bh->b_blockno!=block)
        {
            /*
                other processes might have changed the device or block no

                for their use 
            */
            brelse(bh);
            goto loop;
        }
        
        return bh;
}


struct buffer_head* getblk(int dev,int block){
    /*
        Okay now dealing with 5 scenarios
    
    */
   struct buffer_head* tmp;
   /*
    1. kernel finds the block on its hashqueue and its buffer is free
   */
loop:
    if(tmp=get_b_from_htable(dev,block)){
        remove_from_queues(tmp,REMOVE_FREE); /*
            removing from free list

            because a buffer cannot be both , busy & also on freelist
        */
        tmp->b_lock=true;
        return tmp;
    }
    tmp=freelist;
    /*
        2. Couldn't find a buffer on hashqueue so pick up one(usually first)
        from free list and modify it accordind to new blkno and dev
        and put it on appropriate queue

    */
    /*actually you should be getting first buffer from freelist*/
    do{
        if(!tmp->count){
            wait_on_buffer(tmp);
            if(!tmp->count)
                break;
        }
    }while(tmp!=freelist || tmp!=NULL);
    /*
    
        Scenario 4: no buffers on freelist 
        sleep till any buffer becomes free
    */
    if(!tmp){
        printf("sleping on free buf scenarion 4\n");
        sleep_on(&buffer_wait);
        printf("woke up\n");    
        goto loop;
    }
    tmp->count++;
    remove_from_queues(tmp,REMOVE_FREE); /*remove from freelist*/

    /*
        Scenario 3: buffer marked for delayed write
        so asynchronously write buffer to disk 
        
    */
    if(tmp->b_dirt)
        sync_dev(tmp->b_dev);
    /*
        modify contents
    */
    tmp->b_dev=dev;
    tmp->b_blockno=block;
    tmp->b_dirt=false;
    tmp->b_uptodate=0;

    /*
        
        scenario 5: while you were busy in sync_dev sleeping 
        you may be process B between A & C
        who desreved buffer after released from A but C got it from you
    
    */
   /* somebody might have added new block already, hence check again*/
    if(find_buffer(dev,block)){
        tmp->b_dev=false;
        tmp->b_blockno=0;
        tmp->count=0;
        tmp->b_lock=false;
        insert_into_queues(tmp,INSERT);
        goto loop;
    }
    insert_into_queues(tmp,INSERT);

    return tmp;
}


struct buffer_head* bread(int dev,int block){
    struct buffer_head* bh=NULL;

    if((bh=getblk(dev,block))==NULL)
        printf("bread: fault getting buffer\n");
    if(bh->b_uptodate)
        return bh;
    ll_rw_block(READ,bh);
    if(bh->b_uptodate)
        return bh;
    brelse(bh);

    return NULL;
}


void brelse(struct buffer_head* bh){
    if(bh==NULL)return;
    wait_on_buffer(bh);
    bh->count--;
    wake_up(&buffer_wait);
    if(bh->b_uptodate==true && bh->b_dirt==false){
        insert_into_queues(bh,INSERT_FREE); 

        /*insert at end of free list*/
    }
    else{
        /*
            enqueue at beginning of free list
        */
    }
    if(bh->count==0)
        bh->b_lock=false;
}

void bwrite(struct buffer_head* bh){
    
}

int bmap(struct ii_inode* inode,int offset){
    if(offset >= inode->i_size ){
        puts("issue in bmap , offset > filesize\n");
    }

    int x=offset%BLOCK_SIZE;
    int index=offset-x;
    index=index/BLOCK_SIZE;

    struct buffer_head* bh=getblk(DEVICE,inode->i_block);
    /*
        index 0
        index 1 
    */
   int block=(int)bh->b_data[index];

    /*free bmap arrested block*/
   return block;
}

