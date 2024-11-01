#include<stdlib.h>
#include<stdio.h>
#include"ahci.h"
#include<linux/io.h>

/*
The Port Implemented register (HBA_MEM.pi) is a 32 bit value and 
each bit represents a port. If the bit is set in this register, 
then the corresponding port is present (note: this does not necessarily mean 
a device is attached to the port). A device is attached to a port if the value
of the port's HBA_PxSSTS.det field is 0x3.


*/
#define	SATA_SIG_ATA	0x00000101	// SATA drive
/*
	if any ports signature (port->sig) == SATA_SIG_ATA
	which means 
	31-24: lba high  ==0
	16-23: lba med	==
	8-15: lba low   ==1
	7-0: sector count ==1

which means sata device is in initialisation state.

The signature is update once thr first D2H Register FIS is received.


	

*/

#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	0x96690101	// Port multiplier

#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATA 1
#define AHCI_DEV_SEMB 2
#define AHCI_DEV_PM 3
#define AHCI_DEV_SATAPI 4

#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3

#define AHCI_BASE 0x9432a000

static int check_type(HBA_PORT* port){

	four status=port->ssts;
	
	one ipm = (status>>8 )& 0x0F;   
	//ipm stands for interface power mgmt 
	one det=status & 0x0F;
	// det stands fo device detection
	if(ipm!=HBA_PORT_IPM_ACTIVE){
		return AHCI_DEV_NULL;
	
	}
	if(det!=HBA_PORT_DET_PRESENT){
		return AHCI_DEV_NULL;
	}
	switch(port->sig){
		case SATA_SIG_ATAPI:
			return AHCI_DEV_SATAPI;
		case SATA_SIG_SEMB:
			return AHCI_DEV_SEMB;
		case SATA_SIG_PM:
			return AHCI_DEV_PM;
		default:
			printf("default case its a SATA device\n");
			return AHCI_DEV_SATA;
		
	}


}

void find_disk(HBA_MEM* abar){
	four pi=abar->pi;
	int i=0;
	for(i=0;i<32;i++){
	if(pi& (1<<i)){
		int dt=check_type(&abar->ports[i]);
		if(dt==AHCI_DEV_SATA){
		printf("SATA device found at port %d\n",i);
		}
else if(dt==AHCI_DEV_SATAPI)
	printf("SATAPI device found at port %d\n",i);
else if(dt==AHCI_DEV_SEMB)
	printf("SEMB dev found at port %d\n",i);
else if(dt==AHCI_DEV_PM)
	printf("PM dev found at port %d\n",i);
else
	printf("no drive found at port %d\n",i);


						
	}
}

}
int main(){
	HBA_MEM *abar=(HBA_MEM*)AHCI_BASE;
	find_disk(abar);
	return 0;
}
