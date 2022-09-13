#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/ktime.h>


#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
#define HAVE_PROC_OPS
#endif

static int lab1_show(struct seq_file *m, void *v) {
  int hrs,mins,secs;
  /* some code here */
  return 0;
}

static int lab1_open(struct inode *inode, struct  file *file) {
  return single_open(file, /* some code here */lab1_show, NULL);
}

#ifdef HAVE_PROC_OPS
static const struct proc_ops lab1_fops = {  // For newer Kernels 
  // initializing proc data structure, gets information from 
  /* operation mapping */
};
#else
static const struct file_operations lab1_fops = {
  .owner = THIS_MODULE;
  .open = lab1_open;    //not sure what this is... yet. Only compiled if proc_ops doesn't compile
  .read = seq_read;
  
  /* some code here */
};
#endif

static int __init lab1_init(void) {
  proc_create("lab1",0,NULL,&lab1_fops); // Creates a file in /proc directory on Linux
  /* create proc entry -- done */
  printk(KERN_INFO "lab1mod in\n"); // Logs "lab1mode in\n", meaning intializing, into KERN_INFO
  return 0;
}

static void __exit lab1_exit(void) {
  remove_proc_entry("lab1", NULL)
  /* remove proc entry --done */
  printk(KERN_INFO "lab1mod out\n");
}

MODULE_LICENSE("GPL");
module_init(lab1_init);       // This was changed 
module_exit(lab1_exit);
