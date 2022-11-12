#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/slab.h>

// #include <linux/init.h>
// #include <linux/kernel.h>
// #include <asm/uaccess.h>
// #include <linux/fs.h>

#include "hardlinking_mod.h"
#include "hardlinking_ioctl.h"

MODULE_DESCRIPTION("Simple module");
MODULE_AUTHOR("Kernel Hacker");
MODULE_LICENSE("GPL");

#define DEV_MAJOR       42
#define DEV_MAX_MINORS  5

struct device_data {
    struct cdev cdev;
};

struct device_data devs[DEV_MAX_MINORS];

static int device_hardlink_open(struct inode *inode, struct file *file)
{
    struct device_data *data;
    data = container_of(inode->i_cdev, struct device_data, cdev);
    file->private_data = data;
    printk("device open\n");
	return 0;
}

static long device_hardlink_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
    struct ioctl_link *ioctl_link_paths;
    struct ioctl_unlink *ioctl_unlink_path;
    int ret = 0;

    switch (cmd) {
        case IOCTL_LINK:
            ioctl_link_paths = kmalloc(sizeof(struct ioctl_link), GFP_KERNEL);
            memset(ioctl_link_paths, 0, sizeof(struct ioctl_link));
            ret = copy_from_user(ioctl_link_paths, (struct ioctl_link __user *)arg, sizeof(struct ioctl_link));
            if(!ret){
                ret = link_dir(ioctl_link_paths->src_path, ioctl_link_paths->dst_path);
            }
            kfree(ioctl_link_paths);

            break;
        case IOCTL_UNLINK:
            ioctl_unlink_path = kmalloc(sizeof(struct ioctl_unlink), GFP_KERNEL);
            memset(ioctl_unlink_path, 0, sizeof(struct ioctl_unlink));
            ret = copy_from_user(ioctl_unlink_path, (struct ioctl_unlink __user *)arg, sizeof(struct ioctl_unlink));
            if(!ret){
                ret = unlink_dir(ioctl_unlink_path->path);
            }
            kfree(ioctl_unlink_path);
            break;
    }
    return ret;
}

const struct file_operations device_hardlink_fops = {
    .owner = THIS_MODULE,
    .open = device_hardlink_open,
    .unlocked_ioctl = device_hardlink_ioctl,
};


static int device_hardlink_init(void)
{
	int i, err;
	err = register_chrdev_region(MKDEV(DEV_MAJOR, 0), DEV_MAX_MINORS,
									"device_hardlink_device_driver");
	if (err != 0) {
		return err;
	}

    for(i = 0; i < DEV_MAX_MINORS; i++) {
        cdev_init(&devs[i].cdev, &device_hardlink_fops);
        cdev_add(&devs[i].cdev, MKDEV(DEV_MAJOR, i), 1);
    }
	return 0;
}

static void device_hardlink_exit(void)
{
	int i;
    for(i = 0; i < DEV_MAX_MINORS; i++) {
        cdev_del(&devs[i].cdev);
    }
    unregister_chrdev_region(MKDEV(DEV_MAJOR, 0), DEV_MAX_MINORS);
}

module_init(device_hardlink_init);
module_exit(device_hardlink_exit);

