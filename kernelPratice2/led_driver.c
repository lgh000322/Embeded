#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/gpio.h>


#define GPIO4 (4)

static dev_t my_dev;
static struct cdev my_cdev;

int device_open(struct inode* inode, struct file* filp);
int device_release(struct inode* inode, struct file* filp);
ssize_t device_read(struct file* filp,char* buf,size_t count,loff_t* f_pos);
ssize_t device_write(struct file *filp,const char* buf, size_t count, loff_t* f_pos);

static struct file_operations fops={
    .read=device_read,
    .write=device_write,
    .open=device_open,
    .release=device_release,
};

int device_open(struct inode* inode, struct file* filp)
{
    printk(KERN_INFO "GPIO LED open\n");

    int err;
    int id=GPIO4;
    err=gpio_request(id,"LED");
    if(err)
    {
        printk("LED failed to request GPIO %d\n",id);

    }
        
        
    gpio_direction_output(GPIO4,0);
    return 0;
}

int device_release(struct inode* inode, struct file* filp)
{
    printk(KERN_INFO "GPIO LED release\n");
    gpio_free(GPIO4);
    return 0;
}

ssize_t device_write(struct file* filp, const char *buf,size_t count,loff_t *f_pos)
{
    unsigned char cbuf[10]={0,};
    copy_from_user(cbuf,buf,count);

    printk("Write():GPIO4 to %d.\n",cbuf[0]);

    if(cbuf[0]==1) gpio_set_value(GPIO4,1);
    else gpio_set_value(GPIO4,0);

    return count;
}

ssize_t device_read(struct file* filp,char* buf,size_t count,loff_t *f_pos)
{
    return count;
}

int device_init(void)
{
    if(alloc_chrdev_region(&my_dev,0,1,"my_led_dev"))
    {
        printk(KERN_ALERT "[%s] alloc_chrdev_region failed\n",__func__);
        return -1;
    }

    printk("LED Major=%d, Minor=%d \n",MAJOR(my_dev),MINOR(my_dev));

    cdev_init(&my_cdev,&fops);
    if(cdev_add(&my_cdev,my_dev,1))
    {
        printk(KERN_ALERT "[%s] GPIO is not vaild\n",__func__);
        return -1;
    }

    if(gpio_is_valid(GPIO4)==false){
        printk(KERN_ALERT "[%s] GPIO is not valid.\n",__func__);
        return -1;
    }

    return 0;
}

void device_exit(void)
{
    cdev_del(&my_cdev);
    unregister_chrdev_region(my_dev,1);
}

module_init(device_init);
module_exit(device_exit);
MODULE_LICENSE("GPL");
