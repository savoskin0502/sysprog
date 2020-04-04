#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/hrtimer.h>
//    struct timespec tt;
//    printk(KERN_INFO, getnstimeofday(&tt));

MODULE_LICENSE("GPL");
#define ACTIVATION_TIME 10
struct timespec time;
static struct timer_list mytimer;
static struct hrtimer hr_timer;

// unsigned long delay_in_ms = 1000L;
unsigned long delay_in_ms = 1e9;
int interval = 1000;
int program_counter = 0;
int count = 1;
int last = 0;
int cur = 1;

enum hrtimer_restart my_hrtimer_callback(struct hrtimer *timer)
{
    getnstimeofday(&time);
    printk( "Starting timer to fire in %d s \n", cur);
    printk("TIME hrtimer: %.2lu:%.2lu:%.2lu\r",
                   ((time.tv_sec / 3600) % (24) + 6),
                   (time.tv_sec / 60) % (60),
                   time.tv_sec % 60, time.tv_nsec / 1000);
    
    printk("HRTM Called: [%d]\n",count++);
    
    int temp = last;
    last = cur;
    cur = cur + temp;
    
    ktime_t currtime , interval;
  	currtime  = ktime_get();
  	interval = ktime_set(0, delay_in_ms*cur);
  	hrtimer_forward(timer, currtime , interval);
    return HRTIMER_RESTART;
}

void exampleTimerFunc(struct timer_list *t) {
    getnstimeofday(&time);
    printk("TIME: %.2lu:%.2lu:%.2lu \r\n",
                   ((time.tv_sec / 3600) % (24) + 6),
                   (time.tv_sec / 60) % (60),
                   time.tv_sec % 60);
    program_counter++;
    if(program_counter<=ACTIVATION_TIME){
        mod_timer(&mytimer, jiffies + msecs_to_jiffies(interval));
    }else{
        del_timer(&mytimer);
        printk(KERN_INFO, "Timer enabled.");
        ktime_t ktime;
        printk("HR Timer starting\n");
        ktime = ktime_set( 0, delay_in_ms);
        hrtimer_init( &hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
        hr_timer.function = &my_hrtimer_callback;
        // printk( "Starting timer to fire in %d s \n", cur);
        hrtimer_start( &hr_timer, delay_in_ms, HRTIMER_MODE_REL );
    }
    

}

int init_module(void) {

    printk(KERN_INFO "Starting Timer\n");
    getnstimeofday(&time);
    printk("TIME: %.2lu:%.2lu:%.2lu \r\n",
                   ((time.tv_sec / 3600) % (24) + 6),
                   (time.tv_sec / 60) % (60),
                   time.tv_sec % 60);
    program_counter++;
    timer_setup(&mytimer, exampleTimerFunc, 0);
    mod_timer(&mytimer, jiffies + msecs_to_jiffies(interval));
    

    return 0;
}

void cleanup_module(void) {
    printk(KERN_INFO "Cleanup %s\n");
    del_timer(&mytimer);
    hrtimer_cancel(&hr_timer);
}