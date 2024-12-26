#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#include "checker.h"

#define MAX_TIMERS 10
#define MIN_DELAY 50
#define MAX_DELAY 1000

static unsigned long *delays;
static int num_timers;

static struct timer_list *timers;

static void timer_handler(unsigned long data)
{
  int index = (int)data;

  check_timer();

  if (index + 1 < num_timers)
  {
    mod_timer(&timers[index + 1], jiffies + msecs_to_jiffies(delays[index + 1]));
  }
}

static int __init solution_init(void)
{
  int i;

  if (delays == NULL || num_timers <= 0)
  {
    return -EINVAL;
  }

  for (i = 0; i < num_timers; i++)
  {
    if (delays[i] < MIN_DELAY || delays[i] > MAX_DELAY)
    {
      return -EINVAL;
    }
  }

  timers = kzalloc(sizeof(struct timer_list) * num_timers, GFP_KERNEL);
  if (!timers)
  {
    return -ENOMEM;
  }

  for (i = 0; i < num_timers; i++)
  {
    setup_timer(&timers[i], timer_handler, i);
    if (i == 0)
    {
      mod_timer(&timers[i], jiffies + msecs_to_jiffies(delays[i]));
    }
  }

  return 0;
}

static void __exit solution_exit(void)
{
  int i;

  for (i = 0; i < num_timers; i++)
  {
    del_timer_sync(&timers[i]);
  }

  kfree(timers);
  kfree(delays);
}

module_param(delays, ulong, 0644);
MODULE_PARM_DESC(delays, "Array of delays in milliseconds");
module_param(num_timers, int, 0644);
MODULE_PARM_DESC(num_timers, "Number of timers");

module_init(solution_init);
module_exit(solution_exit);

MODULE_LICENSE("GPL");
