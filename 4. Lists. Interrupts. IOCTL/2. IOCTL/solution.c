#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/init.h>

#define DEVICE_NAME "solution_node"

#define IOC_MAGIC 'k'
#define SUM_LENGTH _IOWR(IOC_MAGIC, 1, char *)
#define SUM_CONTENT _IOWR(IOC_MAGIC, 2, char *)

static dev_t first;
static unsigned int count = 1;
static struct cdev *cdev;
static int major = 240;
static int minor = 0;

static int sum_length = 0;
static int sum_content = 0;

static long solution_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
  char user_buffer[21];
  int ret = 0;

  switch (cmd)
  {
  case SUM_LENGTH:
    if (copy_from_user(user_buffer, (char *)arg, 20))
    {
      return -EFAULT;
    }
    sum_length += strlen(user_buffer);
    ret = sum_length;
    break;
  case SUM_CONTENT:
    if (copy_from_user(user_buffer, (char *)arg, 20))
    {
      return -EFAULT;
    }
    sum_content += simple_strtol(user_buffer, NULL, 10);
    ret = sum_content;
    break;
  default:
    return -EINVAL;
  }

  return ret;
}

static ssize_t solution_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
  int ret;
  char output[64];

  snprintf(output, sizeof(output), "%d %d\n", sum_length, sum_content);

  ret = simple_read_from_buffer(buf, len, offset, output, strlen(output));
  return ret;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = solution_read,
    .unlocked_ioctl = solution_ioctl,
};

static int __init solution_init(void)
{
  first = MKDEV(major, minor);
  register_chrdev_region(first, count, DEVICE_NAME);

  cdev = cdev_alloc();
  cdev_init(cdev, &fops);
  cdev_add(cdev, first, count);

  return 0;
}

static void __exit solution_exit(void)
{
  if (cdev)
  {
    cdev_del(cdev);
  }

  unregister_chrdev_region(first, count);

  return;
}

module_init(solution_init);
module_exit(solution_exit);

MODULE_LICENSE("GPL");
