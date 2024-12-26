#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "solution"

static char *node_name = DEVICE_NAME;
module_param(node_name, charp, 0444);

static dev_t dev_number;
static struct cdev my_cdev;
static struct class *my_class;

static ssize_t mychrdev_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
  char kbuf[20];
  int len = snprintf(kbuf, sizeof(kbuf), "%d\n", MAJOR(dev_number));
  ssize_t nbytes;

  if (*ppos >= len)
  {
    return 0;
  }

  nbytes = min(count, (size_t)(len - *ppos));
  if (copy_to_user(buf, kbuf + *ppos, nbytes))
  {
    return -EFAULT;
  }

  *ppos += nbytes;
  return nbytes;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = mychrdev_read,
};

static int __init solution_init(void)
{
  int ret;

  ret = alloc_chrdev_region(&dev_number, 0, 1, node_name);
  if (ret < 0)
  {
    return ret;
  }

  cdev_init(&my_cdev, &fops);
  ret = cdev_add(&my_cdev, dev_number, 1);
  if (ret < 0)
  {
    unregister_chrdev_region(dev_number, 1);
    return ret;
  }

  my_class = class_create(THIS_MODULE, "solution_class");
  if (IS_ERR(my_class))
  {
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_number, 1);
    return PTR_ERR(my_class);
  }

  if (device_create(my_class, NULL, dev_number, NULL, node_name) == NULL)
  {
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_number, 1);
    return -ENOMEM;
  }

  return 0;
}

static void __exit solution_exit(void)
{
  device_destroy(my_class, dev_number);
  class_destroy(my_class);
  cdev_del(&my_cdev);
  unregister_chrdev_region(dev_number, 1);
}

module_init(solution_init);
module_exit(solution_exit);

MODULE_LICENSE("GPL");
