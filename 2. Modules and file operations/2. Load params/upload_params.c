#include <linux/moduleparam.h>
#include <linux/kobject.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/init.h>

static int a = 0;
static int b = 0;
static int c[5];
static int num_c = 5;

module_param(a, int, 0444);
module_param(b, int, 0444);
module_param_array(c, int, &num_c, 0444);

static int result = 0;
static struct kobject *my_kobject;

static ssize_t my_sys_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
  return sprintf(buf, "%d\n", result);
}

static struct kobj_attribute my_sys_attribute = __ATTR(my_sys, 0444, my_sys_show, NULL);

static int __init solution_init(void)
{
  int error = 0;
  int i;

  result = a + b;
  for (i = 0; i < num_c; i++)
  {
    result += c[i];
  }

  my_kobject = kobject_create_and_add("my_kobject", kernel_kobj);
  if (!my_kobject)
  {
    return -ENOMEM;
  }

  error = sysfs_create_file(my_kobject, &my_sys_attribute.attr);
  if (error)
  {
    kobject_put(my_kobject);
    return error;
  }

  return 0;
}

static void __exit solution_exit(void)
{
  kobject_put(my_kobject);

  return;
}

module_init(solution_init);
module_exit(solution_exit);

MODULE_LICENSE("GPL");