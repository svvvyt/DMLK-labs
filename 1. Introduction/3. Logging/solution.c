#include <linux/module.h>
#include <linux/init.h>
#include <linux/random.h>
#include "checker.h"

MODULE_LICENSE("GPL");

static int __init sum_checker_init(void) {
    CHECKER_MACRO;

    short arrays[10][4];
    int expected_sums[10];
    int i, j, calculated_sum;
    char buf[100];
    ssize_t len;

    for (i = 0; i < 10; i++) {
        expected_sums[i] = 0;
        for (j = 0; j < 4; j++)
        {
            arrays[i][j] = get_random_int() % 10;
            expected_sums[i] += arrays[i][j];
        }
    }

    for (i = 0; i < 10; i++) {
        calculated_sum = array_sum(arrays[i], 4);
        len = generate_output(calculated_sum, arrays[i], 4, buf);

        if (calculated_sum == expected_sums[i]) {
            printk(KERN_INFO "%s", buf);
        }
        else {
            printk(KERN_ERR "%s", buf);
        }
    }

    return 0;
}

static void __exit sum_checker_exit(void) {
    CHECKER_MACRO;

    return;
}

module_init(sum_checker_init);
module_exit(sum_checker_exit);
