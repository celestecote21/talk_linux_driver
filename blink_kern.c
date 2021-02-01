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

static int __init drv_blink_init(void)
{
    LOG("initialisation");
    // allocation of the major and minor dev
    if (register_chrdev_region(dev, 1, "blink_drv_dev_reg") < 0) {
	    LOG("error can't create the dev\n");
        return (-1);
    }
    return (0);
}

static void drv_blink_exit(void)
{
    LOG("exit");
    // free the region in the dev
    unregister_chrdev_region(dev, 1);
}

module_init(drv_blink_init);
module_exit(drv_blink_exit);
