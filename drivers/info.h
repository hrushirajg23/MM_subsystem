/*
    Just information file , i'll add this in 
    .gitignore 


1. FIS_TYPE_REG_H2D (Host to Device Register FIS)



typedef struct tagFIS_REG_H2D {  Fields  } FIS_REG_H2D;

    Purpose: Used by the host to send a command to the device.
    Usage: Whenever the host (your kernel/driver) needs to issue an ATA command to the disk, like a read or write request, it sends this FIS with the command and other register values set.
    Example Commands: Read, Write, Identify Device, and other ATA commands.

2. FIS_TYPE_REG_D2H (Device to Host Register FIS)



typedef struct tagFIS_REG_D2H {  Fields  } FIS_REG_D2H;

    Purpose: Used by the device to update the host on the status of a command.
    Usage: The device sends this type of FIS in response to certain commands, updating the host with information like the device’s status, errors, or other register values.
    Example: After a data transfer, the device may use this FIS to notify the host that the command has completed or to report any errors.

3. FIS_TYPE_DATA (Data FIS)



typedef struct tagFIS_DATA {  Fields  } FIS_DATA;

    Purpose: Used for the actual transfer of data between the host and the device.
    Usage: When the device or host needs to transfer a large data payload (e.g., data being read from or written to disk), this FIS is used to send the actual data payload.
    Direction: Bidirectional, meaning it can be used for both read and write operations.

4. FIS_TYPE_PIO_SETUP (PIO Setup FIS)



typedef struct tagFIS_PIO_SETUP {  Fields  } FIS_PIO_SETUP;

    Purpose: Used by the device to notify the host about a PIO data transfer that’s about to happen.
    Usage: If your disk driver uses Programmed I/O (PIO) mode for data transfer, this FIS tells the host that the device is ready to transfer data.
    Direction: Always from device to host in this setup.
    Note: PIO mode is generally slow and rarely used in modern systems, so this FIS may be unnecessary if you’re using DMA or AHCI modes.

5. FIS_TYPE_DMA_SETUP (DMA Setup FIS)



typedef struct tagFIS_DMA_SETUP {  Fields  } FIS_DMA_SETUP;

    Purpose: Sets up a DMA (Direct Memory Access) transfer.
    Usage: When performing high-speed data transfers, the device can initiate a DMA transfer instead of PIO. This FIS sets up the DMA transfer parameters, including buffer location and size, enabling the device to handle data transfers without frequent CPU intervention.
    Benefits: This is faster and more efficient than PIO because it frees up the CPU to handle other tasks during the data transfer.

Are All of These Needed?

For a minimal driver:

    Mandatory FIS Types: FIS_REG_H2D and FIS_REG_D2H are almost always 
    necessary for basic control and command responses between the host and device.
    Additional for Data Transfers: If you’re doing high-speed data transfers,
     FIS_DMA_SETUP or FIS_DATA is needed.
*/