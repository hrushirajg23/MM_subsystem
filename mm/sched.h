#ifndef _SCHED_H
#define _SCHED_H
/*
    These are i387-math co-processor Data registers

    @docbookinclude:
    https://ohwc.narod.ru/man-dat/cpu/intel/387/387sx.pdf


    Look at page 17 for detailed view of structure

*/



enum region_type {TEXT,DATA,STACK};
#include"head.h"
#include"mm.h"
struct floating{
    long crf;/*control register*/
    long srf;/*status register*/
    long twf;/*tag word register*/
    long fip;/*floating point instruction pointer*/
    long  fcs;
    long foo;/*operand offset*/
    long fos;/*operand selector*/
    long stack[20];
};

struct tss_struct{
    long back_link;
    long esp0;  /*points to top of Kernel Stack,used in privilege level 0(ring 0)*/
    long esp1;
    long esp2;
    long ss0;   /*holds address of Kernel stack*/
    long ss1;   
    long ss2;
    long eax; /*
        int for 64-bits and long for 32-bit systems 

        as we want to store 32 bit register values
        we'll be executing this code on 32bit system long is used == 4 bytes
    */
   /*
   
    remember 16 high-bits are zero
   */
    long ebx;
    long ecx;
    long edx;
    long esp;/*
        esp corresponds to esp3
        points to top of user stack used in privilege level 3(ring 3)
    
    */
    long ebp;/*base pointer*/
    long esi;    /*source index*/
    long edi;    /*destination index*/

    long es;
    long cs;
    long ss;/*
        corresponds to ss3
        holds address of User  stack

    */
    long ds;
    long fs;
    long gs;
    long ldt;
    
    long cr3;    /*control register used for paging*/
    long eip;    /*same as pc(program counter) , next instruction to be executed*/
    long eflags;
    struct floating;
};
typedef void (*pfn)(int);

/*
    @brief: Regions are logical sections of process address space
            3 main regions are text,data,stack
*/
struct e_region{
    enum region_type r_type; /*text,data,or stack*/    
    struct ii_node* i_ptr;/*pointer to elf/object (.o) file*/
    int r_size;/*sizeof region*/
    /*
        Main part i.e of page tables

        I think we must buy a single other page for initalising tables
    
    */
   unsigned long *page_table;
   short status;/*
    locked,
    in_demand,
    being loaded into mem,
    loaded into mem(valid state) ;
   */
    short r_ref;/*reference count , how many processes refer to this region*/

};



/*
    @brief:per process region table
    
*/
struct pregion{
    struct e_region *text;
    struct e_region *data;
    struct e_region *stack;
    unsigned long text_vma;/*address */
    unsigned long data_vma;
    unsigned long stack_vma;
};
/*
    @brief:
    Describes The fields in process table entry
    which are accessible to kernel.

*/
struct e_ptable{
    short state;
    long p_size;
    short prio;
    short pid;
    short ppid;
    //signals sent to process but not yet handled
    //timeers
    struct pregion* preg;
};

/*  
    @brief:
    Describes the fields in u_area that need to be accessible 
    only to runnning process. Kernel should allocate space for u_area
    only when creating a process.

    No need of uareas for process table entries not having processes

    The task corresponds to uarea
*/

struct task{
    /*hardcoded*/
    short state;
    int priority;
    int signal;
    pfn sig_restore;
    pfn sig_fn[32];/*32 types of signals*/
    /*other*/
    int exit_code;
    unsigned long end_text,end_data,brk,start_stack;
    long pid,father,pgrp,session,leader;
    unsigned short uid,euid,suid;
    unsigned short gid,egid,sgid;
    long alarm;
    long utime,stime,cutime,cstime,start_time;
    unsigned short used_math;
    /*file system info*/
    int tty;
    unsigned short umask;
    struct ii_node* pwd;//working dir
    struct ii_node* root;
    unsigned long close_on_exec;
    struct file* filp;/*
    pointer to filetable*/

    struct desc_struct ldt[3];

    struct tss_struct tss;/*register context of a process*/
    
    struct e_ptable p_ptable;/*pointer to ptable entry*/

    int ufdt[50];
    /*
        The below paramter is for copying thisngs from user
        space to kernel space
    */
    unsigned long v_addr_from;
    unsigned long v_addr_to;
    unsigned long size;
    
};

#endif