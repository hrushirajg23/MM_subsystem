#include<stdio.h>
#define BLOCK_SIZE 1024
int main(){


    unsigned long b_end=0x200000;
    unsigned long b_start=1049088;
    int nr_buffers=0;
    long h=b_start;
    while((b_end-=BLOCK_SIZE)>=(h+1)){
        h+=36;
        nr_buffers++;
    }
    printf("total number of buffers are %d\n",nr_buffers);
    printf("last buffer is at %lu\n",h);


    
    return 0;
}