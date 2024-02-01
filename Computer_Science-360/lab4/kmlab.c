#define LINUX
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/workqueue.h>
#include <linux/mutex.h>
#include <asm/uaccess.h>
#include "kmlab_given.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("hutchins"); // Change with your lastname
MODULE_DESCRIPTION("CPTS360 Lab 4");

/************************** Global variables as needed *******************/
#define KMLAB_DIR "kmlab"
struct proc_dir_entry * kmlab_dir;
#define KMLAB_PRIME "status"
struct proc_dir_entry * kmlab_stat;
struct registration_block {
    int pid;
    unsigned long used_time;
    struct list_head next;
};
/********************************************************************/

/*********************** store info ********************************/

LIST_HEAD(registration_list);
static void timer_handler(unsigned long data);
static unsigned long five_sec;
DEFINE_TIMER(update_timer, timer_handler, 0, 0);
static struct workqueue_struct *wq;
#define UPDATE_PRIME "update_wq"
static void work_handler(struct work_struct *work_arg);
struct mutex access_lock;
#define MAX_BUF_SIZE 4096
static int __register_pid(int pid_val, unsigned long used_time) {
    struct registration_block* new_block_ptr;
    struct registration_block* cur, * temp;
    list_for_each_entry_safe(cur, temp, &registration_list, next) {
       if (cur->pid == pid_val) {
	   printk(KERN_ALERT "pid retrace\n");
	   return -EFAULT;
       }
    }
    new_block_ptr = kmalloc(sizeof(*new_block_ptr), GFP_KERNEL);
    new_block_ptr->pid = pid_val;
    new_block_ptr->used_time = used_time;
    INIT_LIST_HEAD(&new_block_ptr->next);
    list_add(&new_block_ptr->next, &registration_list);
    return 0;
}
/********************************************************************/

/********************callback functions for proc read and write**************************/

ssize_t register_pid(struct file *file, 
	const char __user *usr_buf, 
	size_t n, 
	loff_t *ppos) {
    char* kern_buf;
    unsigned long pid_val, used_time;
    int ret;
    kern_buf = (char *)kmalloc(MAX_BUF_SIZE * sizeof(char), GFP_KERNEL);
    if (!kern_buf) 
	return -ENOMEM;
    memset(kern_buf, 0, MAX_BUF_SIZE * sizeof(char));
    if (n > MAX_BUF_SIZE || *ppos > 0) {
	kfree(kern_buf);
	return -EFAULT;
    }
    if (copy_from_user(kern_buf, usr_buf, n)) {
	kfree(kern_buf);
	return -EFAULT;
    }
    kern_buf[n] = 0;
    printk(KERN_DEBUG "ECHO %s", kern_buf); 
    ret = kstrtoul(kern_buf, 10, &pid_val);
    if (ret != 0 || pid_val >= ((1<<16)-1)) {
	kfree(kern_buf);
	printk(KERN_ALERT "unknown pid #");
	return -EFAULT;
    }
    printk(KERN_DEBUG "PID %d", (int)pid_val); 
    ret = get_cpu_use((int)pid_val, &used_time); 
    if (ret != 0) {
	kfree(kern_buf);
	printk(KERN_ALERT "unknown pid #\n");
	return -EFAULT;
    }
    prime_lock(&access_lock);
    if (__register_pid((int)pid_val, used_time) != 0) {
	prime_unlock(&access_lock);
	kfree(kern_buf);
	return -EFAULT;
    }
    prime_unlock(&access_lock);
    kfree(kern_buf);
    return n;
}
/********************************************************************/

/*************************linked list*******************************************/

static void __get_status(int* length, char* kern_buf) {
    struct registration_block* cur, * temp;
    list_for_each_entry_safe(cur, temp, &registration_list, next) {
	*length += sprintf(kern_buf + *length, "PID[%d]: %lu\n", 
				cur->pid, cur->used_time);
    }
}
/********************************************************************/

/*******************************read the proc file /proc/kmlab/status********************/

ssize_t get_status(struct file *file, 
	char __user *usr_buf, 
	size_t n, 
	loff_t *ppos) {
    char* kern_buf;
    int length = 0;
    kern_buf = (char *)kmalloc(MAX_BUF_SIZE * sizeof(char), GFP_KERNEL);
    if (!kern_buf) 
	return -ENOMEM;
    memset(kern_buf, 0, MAX_BUF_SIZE * sizeof(char)); // If input larger than buffer 
    if (n < MAX_BUF_SIZE || *ppos > 0) {
	kfree(kern_buf);
	return 0;
    }
    prime_lock(&access_lock);
    __get_status(&length, kern_buf);
    prime_unlock(&access_lock);
    if (length == 0) {     // If no pid registered in list
	kfree(kern_buf);
	length = sprintf(kern_buf, "No PID registered\n");
    }
    printk(KERN_DEBUG "Read proc file %d\n", length);
    kern_buf[length] = 0;
    if (copy_to_user(usr_buf, (const void *)kern_buf, length)) {//transfer kernel
	kfree(kern_buf);
	return -EFAULT;
    }
    *ppos = length;
    kfree(kern_buf);
    return length;
}
/********************************************************************/

/******************File operations  read, write, open etc*****************/

static const struct file_operations kmlab_proc_fops = {
    .owner = THIS_MODULE,
    .write = register_pid,
    .read = get_status,
};
/********************************************************************/

/*********************update timer***********************************************/
static void timer_handler(unsigned long data) {
    struct work_struct *update_work;
    update_work = (struct work_struct *)kmalloc(sizeof(struct work_struct),
	    		GFP_KERNEL);
    INIT_WORK(update_work, work_handler);
    queue_work(wq, update_work);
    mod_timer(&update_timer, jiffies + five_sec);
}
/********************************************************************/

/**********************update pid**********************************************/
static void __work_handler(void) {
    struct registration_block* cur, * temp;
    int ret;
    list_for_each_entry_safe(cur, temp, &registration_list, next) {
	ret = get_cpu_use(cur->pid, &cur->used_time); 
	if (ret != 0) {
	   list_del(&cur->next);
	   kfree(cur);
	}
    }
}
/********************************************************************/

/*************************workqueue*******************************************/
static void work_handler(struct work_struct *work_arg) {
    prime_lock(&access_lock);
    __work_handler();
    prime_unlock(&access_lock);
    kfree(work_arg);
    #ifdef DEBUG
    printk(KERN_DEBUG "queue finished\n");
    #endif 
}
/**********************kmlab_init - Called when module is loaded******************************/
int __init kmlab_init(void) {
   #ifdef DEBUG
   printk(KERN_ALERT "KMLAB MODULE LOADING\n");
   #endif
   kmlab_dir = proc_mkdir(KMLAB_DIR, NULL); 
   kmlab_stat = proc_create(KMLAB_PRIME, 0666, kmlab_dir, &kmlab_proc_fops); 
   five_sec = msecs_to_jiffies(5000 * 1);
   mod_timer(&update_timer, jiffies + five_sec);
   wq = alloc_workqueue(UPDATE_PRIME, WQ_MEM_RECLAIM, 0);
   mutex_init(&access_lock);
   printk(KERN_ALERT "KMLAB MODULE LOADED\n");
   return 0;   
}
/********************************************************************/

/*************************Remove all entries*******************************************/
static void __kmlab_exit(void) {
    struct registration_block* cur, * temp;
    list_for_each_entry_safe(cur, temp, &registration_list, next) {
       printk(KERN_DEBUG "PID %d: [%lu]\n", cur->pid, cur->used_time);
       list_del(&cur->next);
       kfree(cur);
    }
}
/********************************************************************/

/*****************kmlab_exit - Called when module is unloaded*************************/
void __exit kmlab_exit(void) {
   #ifdef DEBUG
   printk(KERN_ALERT "KMLAB MODULE UNLOADING\n");
   #endif
   proc_remove(kmlab_stat);
   proc_remove(kmlab_dir);
   del_timer(&update_timer);
   if (wq != NULL) {
       flush_workqueue(wq);
       destroy_workqueue(wq);
   }
   prime_lock(&access_lock);
   __kmlab_exit();
   prime_unlock(&access_lock);
   printk(KERN_ALERT "KMLAB MODULE UNLOADED\n");
}
module_init(kmlab_init);
module_exit(kmlab_exit);
/********************************************************************/