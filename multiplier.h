#include <linux/module.h>   // Needed by all modules
#include <linux/moduleparam.h>  /* Needed for module parameters */
#include <linux/kernel.h>   // Needed for KERN_* and printk
#include <linux/init.h>     // Needed for __init and __exit macros
#include <linux/fs.h>	   /* Provides file ops structure */
#include <linux/sched.h>   /* Provides access to the "current" process
			      task structure */
#include <asm/uaccess.h>   /* Provides utilities to bring user space
			      data into kernel space.  Note, it is
			      processor arch specific. */
#include<linux/slab.h>

// Name of the device
#define DEVICE_NAME "multiplier"

// msg_buffer size
#define BUF_LEN 16

char msg_buffer[BUF_LEN];

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

// Major number assigned to the device driver 
static int Major;

/* This structure defines the function pointers to our functions for
   opening, closing, reading and writing the device file.  There are
   lots of other pointers in this structure which we are not using,
   see the whole definition in linux/fs.h */
static struct file_operations fops = {
  .read = device_read,
  .write = device_write,
  .open = device_open,
  .release = device_release
};