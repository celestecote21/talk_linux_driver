#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>

#define LOG(str) (printk(KERN_ALERT "BLINK DRV: %s\n", str))

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Celeste");

dev_t dev = MKDEV(55, 12);
// the class struct will represent the /sys/class of the device
static struct class *dev_class;

static int __init drv_blink_init(void)
{
    LOG("initialisation");
    // allocation of the major and minor dev
    if (register_chrdev_region(dev, 1, "blink_drv_dev_reg") < 0) {
	    LOG("error can't create the dev with major and minor\n");
        return (-1);
    }

    dev_class = class_create(THIS_MODULE, "blink_drv_class");
    if (dev_class == NULL) {
	    LOG("error can't create the class\n");
        goto r_device;
    }

    if (device_create(dev_class, NULL, dev, NULL, "blink_dev") == NULL) {
	    LOG("error can't create the device\n");
        goto r_class;
    }

    return (0);
r_class:
    class_destroy(dev_class);
r_device:
    unregister_chrdev_region(dev, 1); // unalloate the major and minor
    return (-1);
}

static void drv_blink_exit(void)
{
    LOG("exit");
    // destory the device in /dev
    device_destroy(dev_class, dev);
    // destroy the class in /sys/class
    class_destroy(dev_class);
    // free the region in the dev
    unregister_chrdev_region(dev, 1);
}

module_init(drv_blink_init);
module_exit(drv_blink_exit);
