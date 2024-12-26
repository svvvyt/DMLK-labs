#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>

static int my_sys = 0;

static ssize_t my_sys_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
  my_sys++;
  return sprintf(buf, "%d\n", my_sys);
}

static ssize_t my_sys_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
  int ret = kstrtoint(buf, 10, &my_sys);
  if (ret < 0)
  {
    return ret;
  }

  return count;
}

static struct kobj_attribute my_sys_attribute = __ATTR(my_sys, 0664, my_sys_show, my_sys_store);

static struct attribute *attrs[] = {
    &my_sys_attribute.attr,
    NULL,
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

static struct kobject *my_kobject;
static int __init solution_init(void)
{
  my_kobject = kobject_create_and_add("my_kobject", kernel_kobj);
  if (!my_kobject)
  {
    return -ENOMEM;
  }

  int retval;
  retval = sysfs_create_group(my_kobject, &attr_group);
  if (retval)
  {
    kobject_put(my_kobject);
  }

  return retval;
}

static void __exit solution_exit(void)
{
  kobject_put(my_kobject);

  return;
}

module_init(solution_init);
module_exit(solution_exit);

MODULE_LICENSE("GPL");