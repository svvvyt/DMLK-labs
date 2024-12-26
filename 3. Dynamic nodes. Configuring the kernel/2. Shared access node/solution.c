#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/fs.h>

#define DEVICE_NAME "solution_node"
#define KBUF_SIZE (size_t)(256)

struct session
{
  char kbuf[KBUF_SIZE];
  unsigned id;
  int count_req;
  int end_pos;
};

static dev_t first;
static unsigned int count = 1;
static int major = 240;
static int minor = 0;
static struct cdev *cdev;

static int solution_open(struct inode *inode, struct file *file)
{
  static int counter = 0;

  struct session *session = kmalloc(sizeof(struct session), GFP_KERNEL);

  session->id = counter;
  session->count_req = 0;
  memset(session->kbuf, '\0', KBUF_SIZE);
  session->end_pos = sprintf(session->kbuf, "%u", session->id);

  file->private_data = session;

  printk(KERN_INFO "kernel_mooc [OPEN]: sid = %d\n", session->id);

  counter++;

  return 0;
}

static int solution_release(struct inode *inode, struct file *file)
{
  struct session *session = file->private_data;
  printk(KERN_INFO "kernel_mooc [RELEASE]: sid = %d\n", session->id);

  if (session)
  {
    kfree(session);
  }

  session = NULL;
  file->private_data = NULL;

  return 0;
}

static ssize_t solution_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
  struct session *session = file->private_data;
  int nbytes = 0;

  char *kbuf = session->kbuf;
  lbuf = min(lbuf, session->end_pos - *ppos);
  lbuf = lbuf >= 0 ? lbuf : 0;
  nbytes = lbuf - copy_to_user(buf, kbuf + *ppos, lbuf);

  *ppos += nbytes;

  printk(KERN_INFO "kernel_mooc [READ]: ppos = %d, nbytes = %d, lbuf = %d, kbuf = %s\n", *ppos, nbytes, lbuf, kbuf);

  return nbytes;
}

static ssize_t solution_write(struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos)
{
  printk(KERN_INFO "kernel_mooc [WRITE]");

  struct session *session = file->private_data;

  char *kbuf = session->kbuf;

  lbuf = min(lbuf, KBUF_SIZE - *ppos - 1);
  int nbytes = lbuf - copy_from_user(kbuf + *ppos, buf, lbuf);
  *ppos += nbytes;
  session->end_pos = max(session->end_pos, *ppos);

  return nbytes;
}

static loff_t solution_llseek(struct file *file, loff_t offset, int orig)
{
  struct session *session = file->private_data;
  loff_t testpos;

  switch (orig)
  {
  case SEEK_SET:
    testpos = offset;
    break;

  case SEEK_CUR:
    testpos = file->f_pos + offset;
    break;

  case SEEK_END:
    testpos = session->end_pos + offset;
    break;

  default:
    return -EINVAL;
  }

  testpos = testpos < KBUF_SIZE ? testpos : KBUF_SIZE;
  testpos = testpos >= 0 ? testpos : 0;
  file->f_pos = testpos;

  return testpos;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = solution_read,
    .write = solution_write,
    .open = solution_open,
    .release = solution_release,
    .llseek = solution_llseek};

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