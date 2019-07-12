#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <asm/uaccess.h>

/* bug with echo -e "...\n" > file - it is refreshing a file*/

/*ks - kernel space, us - user space*/

MODULE_LICENSE("GPL");
MODULE_LICENCE("GPL v2");

static size_t ks_len = 0;
static struct kobject *my_kobj;
static char *end_combo = "end ";
static char *ks_buff = NULL, *ks_buff_temp = NULL;
static ssize_t sys_show (struct kobject *, struct kobj_attribute *, char *);
static ssize_t sys_store (struct kobject *, struct kobj_attribute *, const char *, size_t);
static const char *sys_dir_name = "ant_sys_dir";

static ssize_t sys_show (struct kobject *kobj, struct kobj_attribute *kobj_attr, char *us_buf) {
    if (!ks_buff) {
	return 0;
    } else {
	strncpy(us_buf, ks_buff, ks_len);
	return strlen(us_buf);
    }
}
static ssize_t sys_store (struct kobject *kobj, struct kobj_attribute *kobj_attr, const char *us_buff, size_t us_len) {
    int offset = 0;

    if (!strncmp(end_combo, us_buff, us_len - 1)) {
	ks_len = 0;
	if (!ks_buff) {
	    kfree(ks_buff);
	    ks_buff_temp = NULL;
	    //printk(KERN_INFO "file refreshed\n");
	}
	return strlen(end_combo);
    }

    ks_len = ks_len + us_len;
    offset = ks_len - us_len;
    ks_buff_temp = krealloc(ks_buff, ks_len, GFP_KERNEL);
    if (ks_buff_temp == NULL) {
	printk(KERN_WARNING "kmalloc can't alloc memory, sys_store fault\n");
	return -EFAULT;
    } else {
	ks_buff = ks_buff_temp;
	strncpy(ks_buff + offset, us_buff, us_len);
	return us_len;
    }
}

//1st arg of __ATTR is stringify
static struct kobj_attribute kobj_attr = __ATTR (ant_sys_file, 0644, sys_show, sys_store);

static int __init sys_init (void) {
	unsigned long res = 0;
	my_kobj = kobject_create_and_add(sys_dir_name, NULL);
	if (!my_kobj) {
	    printk(KERN_WARNING "Can't create sys_dir\n");
	    return -EFAULT;
	}
	res = sysfs_create_file(my_kobj, &kobj_attr.attr);
	if (res) {
	    printk(KERN_WARNING "Can't create sys_file\n");
	    return -EFAULT;
	}
	printk(KERN_INFO "Successfully registered\n");
	return 0;
}

static void __exit sys_exit (void) {
	if (ks_buff != NULL) {
	    kfree(ks_buff);
	}
	kobject_put(my_kobj);
	printk(KERN_INFO "Successfully unregistered\n");
}

module_init (sys_init);
module_exit (sys_exit);