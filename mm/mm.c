#include"mm.h"
#include"sched.h"
#include<stdio.h>
/*

    @brief:
        A memory map denoting each page in memory
*/
static unsigned short mem_map [PAGING_PAGES]={0,};

/*
    Free a page of memory  at physical address 'addr'.
    Used by 'free page tables()';
*/

void free_page(unsigned long addr){
    if(addr<LOW_MEM)return;
    if(addr>HIGH_MEM)printf("dude doesn't exist\n");
    addr-=LOW_MEM;
    /*
        7.9  HIGH_MEM
        7
        6 
        5
        4
        3
        2
        1
        0 base 2mb
    
        to locate the address
    */
   addr>>=12;/*
    to find page of the address
   */
  if((mem_map[addr]--)!=0)return;
  /*
    this page is shared 
  */
  
  mem_map[addr]=0;
  printf("trying to free page\n");

}


unsigned long get_free_page(void){
    register int i=0;
    for(i=0;i<sizeof(mem_map);i++){
        if(mem_map[i]==0){
            return mem_map[i];
        }
    }
    return (unsigned long)0;
}


/*
    
*/
int copy_page_tables(unsigned long from,unsigned long to,long size){

}


void growreg(struct e_region* pentry,int change){
    if(change>0){
        /*
            check whether possible ?
        */
       /*
            allocate pages
       */
      if(pentry->page_table==0){
        pentry->page_table=get_free_page();
        /*
            now pentry->page_table holds address of the physical page frame
        */
        pentry->r_size=0;
      }
      if(pentry->r_size== PAGE_SIZE*sizeof(unsigned long)){
        printf("page table full of entries\n");
      }                   
            long capacity=PAGE_SIZE-(pentry->r_size%PAGE_SIZE);
            long i=0;
            change-=capacity;
            /*
                copy these much to to page frame
            */
            if(change >0){
                pentry->r_size+=capacity;
                /*
                    you need a page
                */
               
               long j=change;
               while(change>0){

             unsigned long page_frame=get_free_page();
            pentry->page_table[((pentry->r_size)/PAGE_SIZE)+i]=page_frame;
            i++;
            change-=PAGE_SIZE;
           }
            pentry->r_size+=j;
          
        }
            else{
                pentry->r_size+=change+capacity;
                /*
                    copy to page frame in loadreg
                */
            }
    }
    else{   /*decreasing*/
        /*
            free page table entries
        */
        long new_s=pentry->r_size+change;
        while(((new_s-(new_s%PAGE_SIZE))/PAGE_SIZE) < ((pentry->r_size-(pentry->r_size%PAGE_SIZE))/PAGE_SIZE)){
            long index=((pentry->r_size-(pentry->r_size%PAGE_SIZE))/PAGE_SIZE);
            mem_map[pentry->page_table[index]]=0;
            pentry->page_table[index]=0;
            pentry->r_size-=PAGE_SIZE;

        }
        pentry->r_size=new_s;
        
        if(pentry->r_size<0){
            pentry->r_size=0;
            mem_map[pentry->page_table[0]]=0;
            pentry->page_table[0]=0;
        }
    }
}

/*
    algorithm: loadreg()
    @brief:
        loads a portion of executable file into a region
    @param:
        1. pointer to per process region table entry
        2. virtual address for executable file
        3. inode pointer for executable file
        4. byte offset in executable file for starting of region
        5. byte count for amount of data to load 
*/
void loadreg(struct e_region* pentry,unsigned long* addr,struct ii_node* inode,unsigned long from,long count){
    /*
        2^32 bytes of addressable physical memory
        if PAGE_SIZE==1024, it becomes 2^22 pages of physical memory
        therefore 

    */
}