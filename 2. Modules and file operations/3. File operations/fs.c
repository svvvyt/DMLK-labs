#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/fs.h>

#define MYDEV_NAME "mychrdev"
#define KBUF_SIZE ((size_t)(10 * PAGE_SIZE))

static dev_t first;
static unsigned int count = 1;
static int my_major = 240;
static int my_minor = 0;
static struct cdev *my_cdev;

static unsigned int M = 0;
static unsigned int N = 0;

static int mychrdev_open(struct inode *inode, struct file *file)
{
  static int counter = 0;
  char *kbuf = kmalloc(KBUF_SIZE, GFP_KERNEL);
  file->private_data = kbuf;

  counter++;
  M++;
  return 0;
}

static int mychrdev_release(struct inode *inode, struct file *file)
{
  char *kbuf = file->private_data;

  if (kbuf)
  {
    kfree(kbuf);
  }

  file->private_data = NULL;
  return 0;
}

static ssize_t mychrdev_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
  char kbuf[100];
  int len = snprintf(kbuf, sizeof(kbuf), "%d %d\n", M, N);
  ssize_t nbytes;

  lbuf = min(lbuf, (size_t)(len - *ppos));
  nbytes = lbuf - copy_to_user(buf, kbuf + *ppos, lbuf);
  *ppos += nbytes;

  printk(KERN_INFO "%d %d\n", M, N);
  return nbytes;
}

static ssize_t mychrdev_write(struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos)
{
  char *kbuf = file->private_data;
  ssize_t nbytes = lbuf;

  if (copy_from_user(kbuf, buf, lbuf))
  {
    return -EFAULT;
  }

  N += nbytes;
  return nbytes;
}

static const struct file_operations mycdev_fops = {
    .owner = THIS_MODULE,
    .read = mychrdev_read,
    .write = mychrdev_write,
    .open = mychrdev_open,
    .release = mychrdev_release};

static int __init init_chrdev(void)
{
  first = MKDEV(my_major, my_minor);
  register_chrdev_region(first, count, MYDEV_NAME);

  my_cdev = cdev_alloc();
  cdev_init(my_cdev, &mycdev_fops);
  cdev_add(my_cdev, first, count);

  return 0;
}

static void __exit cleanup_chrdev(void)
{
  if (my_cdev)
  {
    cdev_del(my_cdev);
  }

  unregister_chrdev_region(first, count);
}

module_init(init_chrdev);
module_exit(cleanup_chrdev);

MODULE_LICENSE("GPL");
