/*

    Assuming memory of 8 mb
*/
#define HIGH_MEM 0x800000 //8mb
#define BUFFER_END 0x200000 //2mb
/*
    BUFFER_END is where buffers will keep their data

*/
//#define BUFFER_START 0x200000  //2mb
#define LOW_MEM BUFFER_END
#define PAGE_SIZE 1024

#define PAGING_MEMORY (HIGH_MEM-LOW_MEM) //8 -2 = 6mb
#define PAGING_PAGES (PAGING_MEMORY/PAGE_SIZE) //assuming each page to be 4k bytes

/*
    6 mb /4096 bytes = 1536 pages

  //  5mb / 1024 bytes(PAGE_SIZE)=.... pages

*/

/*

    MAP(addr) maps the address into its correct page frame.
    it maps addresses only greater than 3mb

*/
#define MAP_NR(addr) (((addr)-LOW_MEM)>>12)


void growreg(struct e_region* pentry,int change);

unsigned long get_free_page(void);


void free_page(unsigned long addr);