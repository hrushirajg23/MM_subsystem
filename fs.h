#ifndef _FS_H
#define _FS_H

#include<stdbool.h>
#include<sys/types.h>

#define READ 0
#define WRITE 1

#define NAMELEN 14
#define TOTAL_INODES 100
#define TOTAL_FILE 50
#define BLOCK_SIZE 1024
#define NR_BUFFERS nr_buffers
#define NR_HASH 307
#ifndef NULL
#define NULL ((void*)0)
#endif

#define BLOCKS_FOR_DISK_ORG 10

/**/
#define MAJOR(a) ((unsigned)(a)>>8) /*The upper 24 bits for major number*/
#define MINOR(a) (a& 0xff) /*clear out upper 24 bits to extract lower 8 bits*/
#define INODES_PER_BLOCK  ((BLOCK_SIZE)/(sizeof(struct d_inode)))

#define DIR_ENTRIES_PER_BLOCK ((BLOCK_SIZE)/sizeof(struct dir_entry ))

typedef char buffer_block[BLOCK_SIZE];

#define DEVICE 4 //randoom

struct buffer_head{
    char* b_data;   
    unsigned short b_blockno;
    unsigned short b_dev;
    bool b_uptodate;
    bool b_dirt;
    unsigned short count;
    bool b_lock;
    /*
        pointer to process which is waiting for this buffer
    */
    struct task* b_wait;
    struct buffer_head* b_prev;
    struct buffer_head* b_next;
    struct buffer_head* b_next_free;
    struct buffer_head* b_prev_free;
};
/*

    Describes te disk inode
*/
struct d_inode{
    unsigned short i_mode;
    unsigned short i_uid;
    unsigned long i_size;
    unsigned long i_time;
    unsigned char i_gid;
    unsigned char i_nlinks;
    unsigned int i_block;
    /*
        indirection block for entries of disk blocks
    */
};

/*
    Describes incore-inode
*/

struct ii_inode{
    /*
        First entries same as dsik inode
    */
    unsigned short i_mode;
    unsigned short i_uid;
    unsigned long i_size;
    unsigned long i_time;
    unsigned char i_gid;
    unsigned char i_nlinks;
    unsigned int i_block; /*
        block number which will be storing 
        256 block entries i.e indirect block
    
    */

    struct task* i_wait;
    unsigned long i_atime;/*accesed time*/
    unsigned long i_ctime;/*changed time*/
    unsigned short i_dev;
    unsigned short i_num;/*indoe num*/
    unsigned short i_count;/*no of file table entries referring to this inode*/
    bool i_lock;
    bool i_dirt;
    bool i_pipe;/*if this is a pipe*/
    bool i_mount;
    bool i_seek;
    bool i_update;
};
/*
    @brief:corresponds to file table entry
*/
struct file{
    unsigned short f_mode;
    unsigned short f_flags;
    unsigned short f_count;/*number of filed desciptors referring to this entry*/
    struct ii_node* f_inode;
    off_t f_pos;
    
};



struct super{
    /*
        Regarding blocks
    */
    int s_fs_size; //size_of_filesystem
    unsigned int s_nfree_blk; //number of freeblocks
  //  unsigned short s_free_blk_ls[50]; //list of free disk block numbers
    unsigned short s_curr_free_block;
    unsigned int s_blk_index; //index of next_free block number
    struct buffer_head* s_fbls[10]; /*free block list*/
    /*
        Regarding inode
    */
    unsigned short s_nfree_inodes; //number of free inodes
    unsigned short s_free_inodes_ls[TOTAL_INODES]; //list of free inodes
    unsigned short s_next_free_inode; //index of next free inode on free inode list
    short remembered_inode;
    /*
        Others
    */
   unsigned short s_dev; //device number
    bool s_locked;
    bool s_mod; //whether super block is modified

};

struct dir_entry{
    unsigned short inode;
    char name[NAMELEN];
};
/*corresponds to 16
    2+14

*/



extern struct ii_node* inode_table[TOTAL_INODES]; //in-core inode table
extern struct file file_table[TOTAL_FILE]; //file table
extern struct super super_block;
extern struct buffer_head* start_buffer;

extern int nr_buffer;


void brelse(struct buffer_head* bh);
struct buffer_head* bread(int dev,int block);
struct buffer_head* getblk(int dev,int block);
void bwrite(struct buffer_head* bh); 
#endif