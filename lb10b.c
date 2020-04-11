#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/gfp.h>
#include <linux/mm_types.h>
#include <linux/types.h>
#include <linux/time.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
// DEFINE_SPINLOCK(lock);
static unsigned int param = 2000;
static struct task_struct *t1;
static struct task_struct *t2;
static int threadFunc(void *args);
module_param(param, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
struct page* mypages;

#define SLABNAME "mycache1"
struct kmem_cache *cache = NULL; 
struct someStruct *p;
struct someStruct {
   unsigned int elems[8];
};

int init_module(void) {

    // struct someStruct st4;

    cache = kmem_cache_create( SLABNAME, sizeof(p), 0, SLAB_HWCACHE_ALIGN, NULL ); 

    t1 = kthread_run(threadFunc, NULL, "mythread1");
    t2 = kthread_run(threadFunc, NULL, "mythread2");
    kthread_stop(t1);
    kthread_stop(t2);
    

   printk(KERN_INFO "Starting2 \n");
   return 0;
}
static int threadFunc(void *args)
{
    
    printk("Param is: %d" ,param);
    struct someStruct sss;
    // printk(KERN_INFO "Starting1 \n");
    printk("Size of struct is: %ld", sizeof(sss));
    

     

    struct timespec tspec1, tspec2;
//    int page_order = 0;
//    int a = 1;
//    while (a < param/128){
//       a *= 2;
//       page_order++;
//    }


    getnstimeofday(&tspec1);
    int i;
    // struct someStruct *p;

    for (i = 0; i<param; i++){
        p = kmem_cache_alloc(cache, GFP_KERNEL);
        kmem_cache_free( cache, p);
        // p = kmalloc(sizeof (struct someStruct), GFP_KERNEL);
        // kfree(p);
        // 
    }
//    mypages = alloc_pages(GFP_KERNEL, page_order);
//    __free_pages(mypages, page_order);
    getnstimeofday(&tspec2);
    printk(KERN_INFO " time required is %ld\n", tspec2.tv_nsec - tspec1.tv_nsec);

   //  printk(KERN_INFO "hello world");
   //  spin_unlock(&lock);
    return 0;
}

void cleanup_module(void) {
    kmem_cache_destroy( cache );
    printk(KERN_INFO "Cleanup \n");
}