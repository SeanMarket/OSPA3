#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

MODULE_AUTHOR("Sean Marketos");
MODULE_LICENSE("GPL");

/*
TODO:
define device buffer size, device name, etc. and other global data structures
 you will need here.
 */
#define BUFFER_SIZE 900
#define DEVICE_NAME "char_driver"

char *kbuff;

int numClose = 0;
int numOpen = 0;

ssize_t char_driver_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
ssize_t char_driver_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);
int char_driver_open(struct inode *pinode, struct file *pfile);
int char_driver_close(struct inode *pinode, struct file *pfile);
loff_t char_driver_seek(struct file *pfile, loff_t offset, int whence);

struct file_operations char_driver_file_operations = {
    .owner = THIS_MODULE,
    .open = char_driver_open,
    .release = char_driver_close,
    .read = char_driver_read,
    .write = char_driver_write,
    .llseek = char_driver_seek,
};

ssize_t char_driver_read(struct file *pfile, char __user *buffer, size_t length,
                         loff_t *offset)
{
  /*
  TODO:
  buffer is the userspace buffer to where you are writing the data you
  want to read from the device file

 length is the length of the userspace buffer

 offset will be set to current position of the opened file after reading
 */
  if (*offset > BUFFER_SIZE)
  {
    printk(KERN_ALERT "Read offset overflow\n");
    return -1;
  }

  if (*offset == BUFFER_SIZE)
  {
    printk(KERN_ALERT "Buffer is full\n");
    return -1;
  }

  int remainingBytes = BUFFER_SIZE - *offset;
  int readBytes = min((int)length, remainingBytes);

  if ((copy_to_user(buffer, kbuff + *offset, readBytes))) // copy to user returns 0 on success
  {
    printk(KERN_ALERT "Copy Error\n");
    return -1;
  }
  *offset += readBytes;
  printk(KERN_ALERT "Read %d bytes", readBytes);
  return readBytes;
}

ssize_t char_driver_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
  /*
TODO:
buffer is the userspace buffer where you are writing the data you want to be
written into the device file

length is the length of the userspace buffer

offset will be set to current position of the opened file
*/
  if (*offset > BUFFER_SIZE)
  {
    printk(KERN_ALERT "Write offset overflow\n");
    return -1;
  }

  if (*offset == BUFFER_SIZE)
  {
    printk(KERN_ALERT "Buffer is full\n");
    return -1;
  }

  int bytesToWrite = 0;

  if (*offset + length > BUFFER_SIZE)
  {
    bytesToWrite = BUFFER_SIZE - *offset;
  }
  else
  {
    bytesToWrite = length;
  }

  if (copy_from_user(kbuff + *offset, buffer, bytesToWrite)) // copy from user returns 0 on success
  {
    printk(KERN_ALERT "Copy Error\n");
    return -1;
  }

  *offset += bytesToWrite;
  printk(KERN_ALERT "Wrote %d bytes\n", bytesToWrite);
  return bytesToWrite;
}

int char_driver_open(struct inode *pinode, struct file *pfile)
{
  /*
  TODO:
  print to the kernel log that the device has been opened, and also print the
   number of times the device has been opened until now.
   */
  if (pfile)
  {
    printk(KERN_ALERT "Device driver opened\n");
    printk(KERN_ALERT "Device has been opened %d prior times\n", numOpen);
    numOpen++;
    return 0;
  }
  return -1;
}

int char_driver_close(struct inode *pinode, struct file *pfile)
{
  /*
  TODO:
   print to the kernel log that the device has been closed, and also print the
   number of times the device has been closed until now.
   */

  if (pfile)
  {
    printk(KERN_ALERT "Device driver closed\n");
    printk(KERN_ALERT "Device has been closed %d prior times\n", numClose);
    numClose++;
    return 0;
  }

  return -1;
}

loff_t char_driver_seek(struct file *pfile, loff_t offset, int whence)
{
  /*
   TODO:
   update the opened file's position according to the values of offset and
   whence.
   */

  switch (whence)
  {
  case 0:
    if (offset < 0 || offset > BUFFER_SIZE)
    {
      printk(KERN_ALERT "Seek offset overflow\n");
      return -1;
    }

    pfile->f_pos = offset;
    break;
  case 1:
    if (pfile->f_pos + offset < 0 || pfile->f_pos + offset > BUFFER_SIZE)
    {
      printk(KERN_ALERT "Seek offset overflow\n");
      return -1;
    }
    pfile->f_pos += offset;
    break;
  case 2:
    if (offset > BUFFER_SIZE)
    {
      printk(KERN_ALERT "Seek offset overflow\n");
      return -1;
    }
    pfile->f_pos = BUFFER_SIZE - offset;
    break;
  default:
    return -1;
  }
  return 0;
}

static int char_driver_init(void)
{
  /*
  //TODO:
   register the device and print to the kernel log that the init function has
   been called.
   */

  register_chrdev(511, DEVICE_NAME, &char_driver_file_operations);
  kbuff = (char *)kmalloc(BUFFER_SIZE, GFP_KERNEL);
  printk(KERN_ALERT "char_driver initialized\n");
  return 0;
}

static void char_driver_exit(void)
{
  /*
   TODO:
   unregister the device using the register_chrdev() function, and print to the
   kernel log that the exit function has been called.
   */
  kfree(kbuff);
  unregister_chrdev(511, DEVICE_NAME);
  printk(KERN_ALERT "char_driver exited\n");
}

/*
 TODO:
 add module_init() and module_exit() functions that point to the corresponding
 init and exit functions of the driver.
 */
module_init(char_driver_init);
module_exit(char_driver_exit);