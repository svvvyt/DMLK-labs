#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/list_sort.h>
#include <linux/slab.h>
#include <linux/interrupt.h>

#define IRQ 8

static struct kobject *kobj;
static int irq_counter = 0;
static int dev_id = 0;

static irqreturn_t interrupt_handler(int irq, void *dev_id)
{
    ++irq_counter;
    return IRQ_HANDLED;
}

static ssize_t my_sys_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", irq_counter);
}
static ssize_t my_sys_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    return 0;
}

static struct kobj_attribute my_sys_attribute = __ATTR(my_sys, 0664, my_sys_show, my_sys_store);

static struct attribute *attrs[] = {
    &my_sys_attribute.attr,
    NULL,
};

static int __init init_solution_module(void)
{
    kobj = kobject_create_and_add("my_kobject", kernel_kobj);
    sysfs_create_file(kobj, attrs[0]);
    request_irq(IRQ, &interrupt_handler, IRQF_SHARED, "my_interrupt", &dev_id);
    return 0;
}

static void __exit exit_solution_module(void)
{
    kobject_put(kobj);
    synchronize_irq(IRQ);
    free_irq(IRQ, &dev_id);
}

MODULE_LICENSE("GPL");
module_init(init_solution_module);
module_exit(exit_solution_module);