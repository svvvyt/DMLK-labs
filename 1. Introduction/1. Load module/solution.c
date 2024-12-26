#include <linux/module.h>

int init_module(void) {
    return 0;
}

void cleanup_module(void) {
    return;
}

MODULE_LICENSE("GPL");