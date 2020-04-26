#include "multiplier.h"


/* This function is run upon module load. This is where you setup data
   structures and reserve resources used by the module */
static int __init my_init(void)
{
  // register the device
  Major = register_chrdev(0, DEVICE_NAME, &fops);

  /* Negative values indicate a problem */
  if (Major < 0) {		
  /* Make sure you release any other resources you've already
      grabbed if you get here so you don't leave the kernel in a
      broken state. */
    printk(KERN_ALERT "Registering char device failed with %d\n", Major);
    return Major;
  }

  // print the major number assigned to the device
  printk("Char device registered with major number %d\n", Major);

  printk(KERN_INFO "Run:\nmknod /dev/%s c %d 0' to register device.\n", DEVICE_NAME, Major);
  
  // A non 0 return means init_module failed; module can't be loaded
  return 0;
}

/* This function is run just prior to the module's removal from the system.
   You should release ALL resources used by your module here (otherwise be
   prepared for a reboot). */
static void __exit my_exit(void)
{
  unregister_chrdev(Major, DEVICE_NAME);
}

static int device_open(struct inode *inode, struct file *file)
{
  printk("Device is opened!\n");
  return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
  printk("Device is closed!\n");
  return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
  int bytes_read = 0;
  
  // return msg_buffer to user
  // character by character
  for (bytes_read = 0; bytes_read < length; bytes_read++){
    put_user((char)msg_buffer[bytes_read], buffer + bytes_read);
  }

  return bytes_read;
}

static ssize_t device_write(struct file *flip, const char __user *buffer, size_t length, loff_t *offset)
{
  int bytes_written;
  char buf;
  int A;
  int B;
  int mul_result;
  int write_to_buffer_index;

  // put the user input into msg_buffer
  // character by character
  for (bytes_written = 0; bytes_written < length; bytes_written ++){
    get_user(buf, buffer + bytes_written);
    msg_buffer[bytes_written] = buf;
  }

  // shift the characters as multiples of 8 
  // then add together to recover the passed-in integer
  A = (msg_buffer[3] << 24) + ((msg_buffer[2] & 0xFF) << 16) + ((msg_buffer[1] & 0xFF) << 8) + (msg_buffer[0] & 0xFF);
  B = (msg_buffer[7] << 24) + ((msg_buffer[6] & 0xFF) << 16) + ((msg_buffer[5] & 0xFF) << 8) + (msg_buffer[4] & 0xFF);

  // multiplication implemented in software
  mul_result = A * B;

  // shift the multiplication result as multiples of 8
  // then store in msg_buffer, starting from index 8
  for (write_to_buffer_index = 0; write_to_buffer_index < sizeof(int); write_to_buffer_index ++){
    msg_buffer[8+write_to_buffer_index] = (mul_result >> (write_to_buffer_index*8)) & 0xFF;
  }

  return bytes_written;
}

// These define info that can be displayed by modinfo
MODULE_LICENSE("GPL");
MODULE_AUTHOR("ECEN449 Student (and others)");
MODULE_DESCRIPTION("Simple multiplier module");

// Here we define which functions we want to use for initialization and cleanup
module_init(my_init);
module_exit(my_exit);
