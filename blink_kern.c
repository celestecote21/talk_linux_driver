#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/gpio.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#define LOG(str) (printk(KERN_ALERT "BLINK DRV: %s\n", str))
#define LOGINFO(str) (printk(KERN_INFO "BLINK DRV: %s\n", str))

#define GPIO_CTRL_OUT 4

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Celeste");

dev_t dev = MKDEV(55, 12);
// the class struct will represent the /sys/class of the device
static struct class *dev_class;

static int blink_pr = 200;

module_param(blink_pr, int, S_IWUSR | S_IWGRP | S_IRUGO);
MODULE_PARM_DESC(blink_pr, "Blink period in ms");

// this is the pointeur to the thread task
static struct task_struct *task;

static int __init drv_blink_init(void)
{
    printk(KERN_INFO "BLINK DRV: Initialisation: blink at %d ms", blink_pr);
    // allocation of the major and minor dev
    if (register_chrdev_region(dev, 1, "blink_drv_dev_reg") < 0) {
	    LOG("error can't create the dev with major and minor\n");
        return (-1);
    }

    // class creation
    dev_class = class_create(THIS_MODULE, "blink_drv_class");
    if (dev_class == NULL) {
	    LOG("error can't create the class\n");
        goto r_device;
    }

    // device creation
    if (device_create(dev_class, NULL, dev, NULL, "blink_dev") == NULL) {
	    LOG("error can't create the device\n");
        goto r_class;
    }

    gpio_request(GPIO_CTRL_OUT, "sysfs");
    // Set the gpio to be in output mode
    gpio_direction_output(GPIO_CTRL_OUT, true);
    // Causes to appear in /sys/class/gpio
    gpio_export(GPIO_CTRL_OUT, false);
    // at the start we put the value at 1 -> the Led will light up
    gpio_set_value(GPIO_CTRL_OUT, 1);

    return (0);

r_class:
    class_destroy(dev_class);
r_device:
    unregister_chrdev_region(dev, 1); // unalloate the major and minor
    return (-1);
}

static void drv_blink_exit(void)
{
    LOGINFO("exit");
    // destory the device in /dev
    device_destroy(dev_class, dev);
    // destroy the class in /sys/class
    class_destroy(dev_class);
    // free the region in the dev
    unregister_chrdev_region(dev, 1);

    // unrealease our GPIO
    gpio_set_value(GPIO_CTRL_OUT, 0);
    gpio_unexport(GPIO_CTRL_OUT);
    gpio_free(GPIO_CTRL_OUT);
}

module_init(drv_blink_init);
module_exit(drv_blink_exit);
