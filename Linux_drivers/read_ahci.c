#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/kernel.h>

#define VENDOR_ID 0x8086  // Replace with actual vendor ID (example: Intel)
#define DEVICE_ID 0x282a  // Replace with actual device ID (example: 82801 Mobile SATA Controller)

// Module parameters
static int my_pci_probe(struct pci_dev *dev, const struct pci_device_id *id);
static void my_pci_remove(struct pci_dev *dev);

static struct pci_device_id ahci_ids[] = {
    { PCI_DEVICE(VENDOR_ID, DEVICE_ID) },
    { 0, }
};

MODULE_DEVICE_TABLE(pci, ahci_ids);

static struct pci_driver ahci_driver = {
    .name = "read_ahci",
    .id_table = ahci_ids,
    .probe = my_pci_probe,
    .remove = my_pci_remove,
};

void __iomem *mmio_base;

static int my_pci_probe(struct pci_dev *dev, const struct pci_device_id *id) {
    unsigned long bar5_addr;

    // Enable the device
    if (pci_enable_device(dev)) {
        printk(KERN_ERR "Failed to enable PCI device\n");
        return -ENODEV;
    }

    // Get BAR5 address
    bar5_addr = pci_resource_start(dev, 5); // Assuming BAR5 is used for AHCI
    mmio_base = ioremap(bar5_addr, 4096);   // Map 4KB

    if (!mmio_base) {
        printk(KERN_ERR "Failed to map BAR5\n");
        return -ENOMEM;
    }

    // Read the AHCI version register (offset 0)
    u32 ahci_version = readl(mmio_base + 0x00);
    printk(KERN_INFO "HRUSHIRAJ MODULE LOADED");
    printk(KERN_INFO "AHCI Version: %x\n", ahci_version);

    return 0; // Success
}

static void my_pci_remove(struct pci_dev *dev) {
    if (mmio_base) {
        iounmap(mmio_base); // Unmap the memory
        mmio_base = NULL;
    }
    printk(KERN_INFO "AHCI module unloaded\n");
}

static int __init read_ahci_init(void) {
    return pci_register_driver(&ahci_driver); // Register the PCI driver
}

static void __exit read_ahci_exit(void) {
    pci_unregister_driver(&ahci_driver); // Unregister the PCI driver
}

module_init(read_ahci_init);
module_exit(read_ahci_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Simple AHCI Driver");
