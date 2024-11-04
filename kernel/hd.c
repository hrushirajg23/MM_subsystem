#include<fs.h>
#include<mm.h>
#include<include/meow/config.h>
#include"include/asm/system.h"
#include<stdio.h>
/*
    MAJOR number repersents the device and minor number represents the parition 
    on that device  

*/
#define NR_REQUEST 32
#define MAX_HDS 1
//{7,35,915,65536,920,0}
static struct hd_info_struct{
    int head,sec,cyl,wpcom/*write precompensation*/,lzone,ctl;
}hd_info[]={HD_TYPE};

//is equal to 2,23,232 blocks of size 512 bytes

static struct hd_request* this_request=NULL;

static bool working=false;

static struct task* wait_for_request=NULL;

static struct hd_struct{
    long start_sect;
    long nr_sects;
}hd[5*MAX_HDS]={{0,0},};

/*
    hq_reuqest is a linked list of requests 
    that'll be using elevator block algorithm
*/

static inline lock_buffer(struct buffer_head* bh){
    if(bh->b_lock==true){
        printf("already locked\n");
    }
    bh->b_lock=true;
}
static inline unlock_buffer(struct buffer_head* bh){
    if(bh->b_lock==false){
        printf("already unlocked\n");
    }
    bh->b_lock=false;
    wake_up(&bh->b_wait);/*
        wakeup all processes waiting for a buffer or for this particular buffer
    
    */
}


#define RIGHT_POS(r1,r2)\
((r1)->head <= (r2)->head && \
((r1)->cyl <= (r2)->cyl && \
((r1)->sector <= (r2)->sector) ))

static struct hd_request{
    int hd;
    int nsector;
    int sector;
    int head;
    int cyl;
    int cmd;
    int errors;
    struct buffer_head* bh;
    struct hq_request* next;
}request[NR_REQUEST];

struct CHS{
    int cyl;
    int head;
    int sector;
};

static inline void convert_chs(int blkno,struct CHS* chs_,struct hd_info_struct* info){
    chs_->sector=blkno%info->sec; /*block number % no of sectors per track*/
    int track_of_cyl0=blkno/info->sec; /*look into notebook 
    
        small explanation
    ok i'll write later , focus on code 
    */
   chs_->cyl=track_of_cyl0/info->head; 
   chs_->head=blkno%info->head;
}

void rw_hd(int rw,struct buffer_head* bh){
    unsigned int block,dev;
    unsigned int sec,head,cyl;

    block=bh->b_blockno<<1; 
    dev=MINOR(bh->b_dev);

    /*
        hard disk's block size is 512 bytes and logical block size is 1024 bytes
        As two 512 byte blocks = 1 logical block

        0 512 1024 1536 2048
        0 1     2   3   4

        0 1024 2048
        0 1     2

        so jo logical cha asel * 2=disk block
        if bh->blockno=2 ,therefore logical = 2*2= 4;
        we get address 2048.
    */
   /*
        block is corresponds to sector 
        hence
   */
    block+=hd[dev].start_sect;
    dev/=5;
    /*
        look into notebook for chs conversion
    */
    struct CHS chs;
    convert_chs(block,&chs,&hd[dev]);

    rw_abs_hd(rw,dev,chs.cyl,chs.head,chs.sector+1,bh);
}


void add_request(struct hd_request* req){
    struct hd_request* temp=NULL;

    /*
        no of sectors to read/write must be two as 
        buffer_size =1024 and and sector size =512
        so we need atleast 2 sectors to write/read a buffer hence nsector=2
    */
    if(req->nsector!=2){
        printf("nsector must be 2\n");
    }
    /*
        first two entries in list are preserved 
    */
    working=true;/*don't allow interrupts*/
    if((temp=this_request)==NULL)this_request=req;
    else{
        if(temp->next==NULL)temp->next=req;

        /*
            put the request at correct position by comparing theirs chs's
            if one has chs less than other its put before .
            this is done to improve disk efficiency
        */
        else{
            cli();
            
            /*
                not touching
            */
           temp=temp->next;
            for(;temp->next!=NULL;temp=temp->next){
                if((RIGHT_POS(temp,req) || !RIGHT_POS(temp,temp->next)) &&
				    RIGHT_POS(req,temp->next))break;
                /*        temp -> (temp->next)
                    e.g     30 ->       50           
                    and I want to insert 40
                    RIGHT_POS(30,40 ==true) 
                */
            }
            req->next=temp->next;
            temp->next=req;
            sti();
        }
    }
    working=0;

    if(!do_hd)
        do_request();
}

void rw_abs_hd(int rw,int dev,int cyl,int head,int sector,struct buffer_head* bh){
    struct hd_request* rq=NULL;

    /*read kinvha write donhi nasel tar*/
    if(rw!=READ && rw!=WRITE){
        printf("must be read or write\n");
    }
    lock_buffer(bh);
loop:
    for(rq=0+request;rq<NR_REQUEST+request;rq++){

    }


}


/*
    This function creates a linked list of blocks
    in memory. each block will contain 256 entries 
    and last entry will contain address of next block
    containing other 256 entries and so on.

*/
void organize_disk(){

   
    /*
        lets assume boot block and super block takes total 2 blocks
        i.e first 2 blocks(0-1) assume size of eac block to be 1024 bytes
        in correspondence to buffer hence 1 buffer is needed for boot and super blk.

        /*
            and next 10 blocks will be holding in memory data of disk lists
            so total 12 blocks are used by filesys. 

            as sector size = 512 and buffer size = 1024 

            buffers needed to write  are 6

            hence blocks start from 12 while others will be hardcoded. 
        */
    if(super_block.s_fs_size==0){
        super_block.s_blk_index=0;
        super_block.s_curr_free_block=0;
    }
    int blkno=12;
    for(int i=2;i<=11;i++){
        
        struct buffer_head* bh=getblk(DEVICE,i);
        if(bh==NULL){
            printf("organizing disk failed\n");
        }
        super_block.s_fbls[i-2]=bh;

        if(super_block.s_fs_size==0){
            /*means installing system first time on a device
                we need to organize lists

            */  
           /*
            this is ideal calculation but but
            since 10 
             int total_blks=hd_info[0].cyl*hd_info[0].head*hd_info[0].sec;     
           */

            int* ptr=(int*)bh->b_data;
            for(int j=0;j<BLOCK_SIZE/sizeof(int);j++){
                *ptr=blkno++;        
                ptr++;
            }
            
        }

    }
    if(super_block.s_fs_size==0){
        super_block.s_nfree_blk=blkno-12;
    }
    
}



int alloc(int dev){
    if(super_block.s_blk_index==BLOCK_SIZE/sizeof(int)){
        if(super_block.s_curr_free_block==BLOCKS_FOR_DISK_ORG){
            printf("blocks evacutated\n");
            return -1;
        }
        super_block.s_curr_free_block++;
        super_block.s_blk_index=0;
    }
    printf("block number %d and index number %d\n",super_block.s_curr_free_block,super_block.s_blk_index);
    super_block.s_blk_index++;
    return (int)super_block.s_fbls[super_block.s_curr_free_block]->b_data[super_block.s_blk_index-1];
}

void free(int blkno){
    if(super_block.s_blk_index==0 ){
        if(super_block.s_curr_free_block==2){
            printf("failed organizing disk :free() \n");
            return;
        }
        super_block.s_curr_free_block--;
        super_block.s_blk_index=(BLOCK_SIZE/sizeof(int))-1;
    }
    else{
        super_block.s_blk_index--;
    }
    int* ptr=(int*)&super_block.s_fbls[super_block.s_curr_free_block]->b_data[super_block.s_blk_index];
    *ptr=blkno;
}



/*

    blk 0 { 1 2 3 4 5 -------------------

    blk 1 {100 101 102 103 104 ------------------}
*/