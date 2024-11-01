Memory layout

/*



    --------------------2mb
    |      |             
    |-------------------|                   
    |     |              
    |-------------------|                  
    |   |                
    |-------------------|                   
    |                   |
    |                   |   
    |  |           
    |                   |
    |                   |   
    |                   |
    |-------------------|                   
    | 36*100=3600bytes                         
    |                   
    |                   

    --------------------1mb
    |  64kb boot rom    |             
    |-------------------|                   
    |192kb bios rom     |              
    |-------------------|                  
    |128kb video rom    |                
    |-------------------|                   
    |                   |
    |                   |   
    |kernel(read mode)  |           
    |                   |
    |                   |   
    |                   |
    |-------------------|                   
    |  2kb bios data    |                    
    |-------------------|                   
    | 1kb IVT           |       
    ---------------------0mb
    

*/


/*


I/O information
We mention this distinction between controller and device because the operat-
ing system nearly always deals with the controller, not the device. Most personal
computers and servers use the bus model of Fig. 3-2 for communication between
the CPU and the controllers. 

    So apan deal karnar ahot with controllers 
2.
The controller’s job is to convert the serial bit stream into a block of bytes and
perform any error correction necessary. The block of bytes is typically first as-
sembled, bit by bit, in a buffer inside the controller. After its checksum has been
verified and the block declared to be free of errors, it can then be copied to main
memory.



    MEMORY-MAPPED I/O

    The Pentium uses this architecture, with addresses 640K to 1M
being reserved for device data buffers in IBM PC compatibles, in addition to I/O
ports 0 through 64K.

Pentium hybrid architecture vaparto, it can control devices from 
memory-map and also from cpu registers resereved for I/O


In addition to status bits, many controllers use interrupts to tell the CPU when
they are ready to have their registers read or written. 


    Direct Memory Access (DMA)
Whether or not a system has memory-mapped I/O, its CPU needs to address
the device controllers to exchange data with them. The CPU can request data from
an I/O controller one byte at a time but doing so for a device like a disk that pro-
duces a large block of data wastes the CPU’s time, so a different scheme, called
DMA (Direct Memory Access) is often used

No matter where it is physically located, the DMA controller has access to the
system bus independent of the CPU, as shown in Fig. 3-4. It contains several
registers that can be written and read by the CPU

These include a memory
address register, a byte count register, and one or more control registers. The control registers specify the I/O port to use, the direction of the transfer (reading from
the I/O device or writing to the I/O device), the transfer unit (byte at a time or
word at a time), and the number of bytes to transfer in one burst.

*/

/*



     If we have only physical 8 heads why are logical heads specified to 255 ?
    ANS:It allows to address more storage 
    the logical head no is % 8 to match to physical head number.
    Compatibility with BIOS and Legacy Systems: Many BIOS and operating systems were designed to accommodate a maximum of 255 heads for compatibility with older disk standards, even if the physical hardware has fewer heads.

     If you have a disk with 8 physical heads, the logical addressing might be designed as follows:

    Logical Head 0 → Physical Head 0
    Logical Head 1 → Physical Head 1
    Logical Head 2 → Physical Head 2
    Logical Head 3 → Physical Head 3
    Logical Head 4 → Physical Head 4
    Logical Head 5 → Physical Head 5
    Logical Head 6 → Physical Head 6
    Logical Head 7 → Physical Head 7
    Logical Head 8 → Physical Head 0 (and so on)
