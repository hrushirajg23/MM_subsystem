
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



static inline void wait_on_buffer(struct buffer_head* bh){
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

static void inline insert_into_queues(struct buffer_head* bh){
    /*
        put at end of free list
    */
    bh->b_next_free=freelist;
    bh->b_prev_free=freelist->b_prev_free;
    
    /*freelist chya tail chya pudhe adding new bh*/
    freelist->b_prev_free->b_next_free=bh;
    freelist->b_prev_free=bh;


    /*
        putting in hashqueue
    */
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


static inline void remove_from_queues(struct buffer_head* bh){

/*removing from hash-queue*/
    if(bh->b_next)
        bh->b_next->b_prev=bh->b_prev;
    if(bh->b_prev)
        bh->b_prev->b_next=bh->b_next;
    if(hash(bh->b_dev,bh->b_blockno)==bh) /*only single  or the first element*/
           bh->b_next->b_prev=bh->b_next;    

/*
    remove from free list
*/    

    if((bh->b_prev_free==NULL) || (bh->b_next_free=NULL)){
        printf("freelist sucks\n");
    }

    bh->b_prev_free->b_next_free=bh->b_next_free;
    bh->b_next_free->b_prev_free=bh->b_prev_free;
    if(freelist==bh)
        freelist=bh->b_next;

}

static  struct buffer_head* find_buffer(int dev,int block){
    struct buffer_head* temp=NULL;
    for(temp=hash(dev,block);temp!=NULL;temp++){
            if(temp->b_dev==dev && temp->b_blockno==block)
                return temp;
    }
    return NULL;
}
