#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/spinlock.h>

MODULE_LICENSE("GPL");
DEFINE_SPINLOCK(lock);

#define MAX_NUM 1000
static struct task_struct *t1;
static struct task_struct *t2;
static struct task_struct *t3;
static struct task_struct *t4;

int unsorted[MAX_NUM];
int sorted[MAX_NUM];

static int merge(void *args);
static int threadSort(void *args);

struct mergeParams {
    int begin;
    int mid;
    int end;
};

struct insParams {
    int from;
    int to;
};

int gen_int(void) {
    int n;
    get_random_bytes(&n, sizeof(n));
    n = n < 0 ? n * -1 : n;
    n = n % MAX_NUM;
    return n;
}

int generateNum(void){
    int n;
    n = gen_int();
    return n;
}

int init_module(void)
{
    int i;
    for (i = 0;i < MAX_NUM; i++){
        unsorted[i] = generateNum();
    }
    int from1 = 0;
    int to1 = MAX_NUM/2;
    int from2 = to1+1;
    int to2 = MAX_NUM-1;

    struct insParams sArgs[2];
    sArgs[0].from = from1;
    sArgs[0].to = to1;
    t1 = kthread_run(threadSort, &sArgs[0], "mythread1");

    sArgs[1].from = from2;
    sArgs[1].to = to2;
    t2 = kthread_run(threadSort, &sArgs[1], "mythread2");
    
    struct mergeParams mArgs;
    mArgs.begin = from1;
    mArgs.mid = from2;
    mArgs.end = to2;
    t3 = kthread_run(merge, &mArgs, "mythread3");
    t4 = kthread_run(merge, &mArgs, "mythread4");
    kthread_stop(t1);
    kthread_stop(t2);
    kthread_stop(t3);
    kthread_stop(t4);
    return 0;
}

static int merge(void *args) {
    spin_lock(&lock);
    struct mergeParams *params = (struct mergeParams*) args;
    int begin = params->begin,
        mid = params->mid,
        end = params->end;

    int i = begin, j = mid, tpos = begin;

    while (i < mid && j <= end)
    {
        if (unsorted[i] < unsorted[j])
            sorted[tpos++] = unsorted[i++];
        else
            sorted[tpos++] = unsorted[j++];
    }
    while (i < mid)
        sorted[tpos++] = unsorted[i++];
    while (j <= end)
        sorted[tpos++] = unsorted[j++];
    printk(KERN_INFO "started is here");
    for (i = 0; i <  MAX_NUM; i++){
        printk(KERN_INFO "rand arr num: %d\n",sorted[i] );
    }
    // for ( i = MAX_NUM - 10; i < MAX_NUM-1;i++){
    //     printk(KERN_INFO "rand arr num: %d\n",sorted[i] );
    // }
    spin_unlock(&lock);
    return 0;
}

static int threadSort(void *args)
{
    spin_lock(&lock);
    struct insParams *params = (struct insParams*) args;
    int from = params->from,
    to = params->to;

    int i = from, j, itemToInsert;
    while (i <= to)
    {
        itemToInsert = unsorted[i];
        j = i-1;
        while (j >= from && itemToInsert < unsorted[j])
        {
            unsorted[j+1] = unsorted[j];
            j--;
        }
        unsorted[j+1] = itemToInsert;
        i++;
    }
    spin_unlock(&lock);

    return 0;
}

void cleanup_module(void)
{    
    
}
