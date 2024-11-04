#include<mm.h>
#include<fs.h>
#include<elf.h>
#include<sched.h>

/*
    Including stdlib and stdio 
    for testing purposes only
*/

#include<stdio.h>
#include<stdlib.h>


/*pointer to executable file*/
void load_segments(struct ii_inode* inode){
    /*
    
    10 blocks of 1024 bytes                         10240 bytes = 10kb
    1 single indirect block of 256 direct blocks    256*1024bytes = 262,144 = 256kb
    1 double indirect block of 256 indirect blocks 

    so if we include only single indirect block without 10 direct its
    it will be more better and 256kb enough for testing elfs.
    */
    

}