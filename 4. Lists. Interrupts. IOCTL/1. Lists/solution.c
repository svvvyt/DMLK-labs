#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/string.h>
#include <linux/mutex.h>

static struct kobject *my_kobject;

struct module_name_node
{
  char name[MODULE_NAME_LEN];
  struct list_head list;
};

static LIST_HEAD(module_name_list);
static DEFINE_MUTEX(module_list_mutex);

static int populate_module_name_list(void)
{
  struct module *mod;
  struct module_name_node *node, *tmp;

  mutex_lock(&module_list_mutex);

  list_for_each_entry_safe(node, tmp, &module_name_list, list)
  {
    list_del(&node->list);
    kfree(node);
  }

  list_for_each_entry(mod, THIS_MODULE->list.prev, list)
  {
    node = kmalloc(sizeof(*node), GFP_KERNEL);
    if (!node)
    {
      mutex_unlock(&module_list_mutex);
      return -ENOMEM;
    }
    strncpy(node->name, mod->name, MODULE_NAME_LEN);
    INIT_LIST_HEAD(&node->list);

    struct module_name_node *pos;
    bool inserted = false;
    list_for_each_entry(pos, &module_name_list, list)
    {
      if (strcmp(node->name, pos->name) < 0)
      {
        list_add_tail(&node->list, &pos->list);
        inserted = true;
        break;
      }
    }
    if (!inserted)
    {
      list_add_tail(&node->list, &module_name_list);
    }
  }

  mutex_unlock(&module_list_mutex);
  return 0;
}

static ssize_t my_sys_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
  struct module_name_node *node;
  size_t len = 0;

  populate_module_name_list();

  mutex_lock(&module_list_mutex);
  list_for_each_entry(node, &module_name_list, list)
  {
    len += scnprintf(buf + len, PAGE_SIZE - len, "%s\n", node->name);
  }
  mutex_unlock(&module_list_mutex);

  return len;
}

static struct kobj_attribute my_sys_attr = __ATTR_RO(my_sys);

static int __init solution_init(void)
{
  int retval;

  my_kobject = kobject_create_and_add("my_kobject", kernel_kobj);
  if (!my_kobject)
    return -ENOMEM;

  retval = sysfs_create_file(my_kobject, &my_sys_attr.attr);
  if (retval)
  {
    kobject_put(my_kobject);
    return retval;
  }

  return 0;
}

static void __exit solution_exit(void)
{
  struct module_name_node *node, *tmp;

  mutex_lock(&module_list_mutex);
  list_for_each_entry_safe(node, tmp, &module_name_list, list)
  {
    list_del(&node->list);
    kfree(node);
  }
  mutex_unlock(&module_list_mutex);

  sysfs_remove_file(my_kobject, &my_sys_attr.attr);
  kobject_put(my_kobject);
}

module_init(solution_init);
module_exit(solution_exit);

MODULE_LICENSE("GPL");