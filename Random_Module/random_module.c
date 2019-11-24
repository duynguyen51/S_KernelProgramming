#include <linux/init.h>             // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>           // Core header for loading LKMs into the kernel
#include <linux/device.h>           // Header to support the kernel Driver Model
#include <linux/kernel.h>           // Contains types, macros, functions for the kernel
#include <linux/fs.h>               // Header for the Linux file system support
#include <linux/uaccess.h>          // Required for the copy to user function
#include <linux/slab.h>             // Required for kmalloc/kfree functions
#include <linux/random.h>           // Required for the get_random_bytes function


#define DEVICE_NAME "random_module_device"


static int major; // Global variable for the first device number
static struct class *cl; // Global variable for the device class

static unsigned int c;
static unsigned int count_opened = 0;

static int my_open(struct inode *i, struct file *f)
{
	count_opened += 1; 
	printk(KERN_INFO "Driver: open() ----- %i times\n", count_opened);
	return 0;
}
static int my_close(struct inode *i, struct file *f)
{
	printk(KERN_INFO "Driver: close()\n");
	return 0;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PHAN HUU TU");
MODULE_DESCRIPTION("My First Character Driver"); 

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
	
	c = 0;
	get_random_bytes(&c, sizeof(int));
	c &= 0x7FFFFFFF;
	
	printk(KERN_INFO "Driver: read() : %i\n, i");
	
	if (copy_to_user(buf, &c, 1) != 0)
	{
		printk(KERN_INFO "Driver: Failed to send a randomed number to user!!!\n");
		return -EFAULT;
	}
	printk(KERN_INFO "Driver: Already sent a randomed number to user!!!\n");
	return 0;
}
static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "Driver: write() :\n");
	return 0;
}

static struct file_operations pugs_fops = { 
	.owner = THIS_MODULE, 
	.open = my_open, 
	.release = my_close, 
	.read = my_read, 
	.write = my_write
};

static int __init ofcd_init(void) /* Constructor */
{
	printk(KERN_INFO "Hello: ofcd registered");

	major = register_chrdev(0, DEVICE_NAME, &pugs_fops);
    if (major < 0){
        printk(KERN_ALERT "Driver: Failed to register a major number\n");
        return major;
    }
	if ((cl = class_create(THIS_MODULE, "character_random_module_driver")) == NULL)
	{
		unregister_chrdev(major, DEVICE_NAME);
		return -1;
	}
	if (device_create(cl, NULL, MKDEV(major, 0), NULL, DEVICE_NAME) == NULL)
	{
		class_destroy(cl);
		unregister_chrdev(major, DEVICE_NAME);
		return -1;
	}

	printk(KERN_INFO "Driver: Device class created correctly\n"); 
	return 0;
}
static void __exit ofcd_exit(void) /* Destructor */
{
	device_destroy(cl, MKDEV(major, 0));
	class_unregister(cl);
	class_destroy(cl);
	unregister_chrdev(major, DEVICE_NAME);
	printk(KERN_INFO "Goodbye Lab!!!\n");
}
module_init(ofcd_init);
module_exit(ofcd_exit);