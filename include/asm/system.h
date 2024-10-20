#define sti() __asm__  ("sti"::) //enable interrupts
#define cli() __asm__ ("cli"::) //disable interrupts
#define nop() __asm__ ("nop"::) //no operation ,move to next instruction