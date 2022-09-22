#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h> // include sched.h to implement process control block
#include <linux/proc_fs.h>
#include <linux/cred.h> 
#include <linux/seq_file.h>
#include <linux/ktime.h>


#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
#define HAVE_PROC_OPS
#endif

static int lab1_show(struct seq_file *m, void *v) {
  //sched_
  /* some code here */
  // get PID from sched.h data structure
  struct task_struct * cur_task = current;

  // get state from cur_task data structure
  int state = current -> state;  
  
  // get PID and PPID from sched.h data structure
  int PID = current->pid;

  // get Real UID from cred.h data structure
  int RealUID = current -> cred -> uid;

  seq_printf(m,"Current Process PCB Information\n");
  seq_printf(m,"PID = %d\n", PID);

  seq_printf(m, "Real UID = %d\n", RealUID);
  return 0;

}

static int lab1_open(struct inode *inode, struct  file *file) {
  return single_open(file,lab1_show, NULL); // point to lab1_show when lab1 file is opened
}

#ifdef HAVE_PROC_OPS
static const struct proc_ops lab1_fops = { 
  .proc_open = lab1_open,
  .proc_read = seq_read,
  .proc_lseek = seq_lseek,
  .proc_release = single_release,

   // For newer Kernels 
  // initializing proc data structure, gets information from 
  /* operation mapping */
};
#else
static const struct file_operations lab1_fops = {
  .owner = THIS_MODULE,
  .open = lab1_open,
  .read = seq_read,
  .llseek = seq_lseek,
  .release = single_release,        //done

  
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
  remove_proc_entry("lab1", NULL);      // Remove proc entry
  printk(KERN_INFO "lab1mod out\n");
}

MODULE_LICENSE("GPL");
module_init(lab1_init);       // This was changed 
module_exit(lab1_exit);
