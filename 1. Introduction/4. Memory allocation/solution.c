#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include "checker.h"

MODULE_LICENSE("GPL");

void* void_ptr = NULL;
int* int_array_ptr = NULL;
struct device* device_ptr = NULL;

static int __init solution_init(void) {
    ssize_t bytes_count = get_void_size();
    void_ptr = kmalloc(bytes_count, GFP_KERNEL);
    submit_void_ptr(void_ptr);

    ssize_t int_array_size = get_int_array_size();
    int_array_ptr = kmalloc(int_array_size * sizeof(int), GFP_KERNEL);
    submit_int_array_ptr(int_array_ptr);

    device_ptr = kmalloc(sizeof(struct device), GFP_KERNEL);
    submit_struct_ptr(device_ptr);

    return 0;
}

static void __exit solution_exit(void) {
    checker_kfree(void_ptr);
    checker_kfree(int_array_ptr);
    checker_kfree(device_ptr);
    
    return;
}

module_init(solution_init);
module_exit(solution_exit);
