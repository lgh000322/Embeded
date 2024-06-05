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
#include <linux/delay.h>

#define GPIO4 (4)
#define GPIO22 (22)
#define GPIO23 (23)

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
    int id1=GPIO4;
    err=gpio_request(id1,"Output");
    if(err)
    {
        printk("Output failed to request GPIO %d\n",id1);
    }    
    gpio_direction_output(GPIO4,0);

    int id2=GPIO22;
    err=gpio_request(id2,"Latch");
    if(err)
    {
        printk("Latch failed to request GPIO %d\n",id2);
    }
    gpio_direction_output(GPIO22,0);

    int id3=GPIO23;
    err=gpio_request(id3,"Shfit");
    if(err)
    {
        printk("Shfit failed to request GPIO %d\n",id3);
    }
    gpio_direction_output(GPIO23,0);

    return 0;
}

int device_release(struct inode* inode, struct file* filp)
{
    printk(KERN_INFO "GPIO LED release\n");
    gpio_free(GPIO4);
    gpio_free(GPIO22);
    gpio_free(GPIO23);
    return 0;
}

ssize_t device_write(struct file* filp, const char *buf,size_t count,loff_t *f_pos)
{
    unsigned char cbuf[8]={0,};
    if (copy_from_user(cbuf, buf, count)) {
    printk(KERN_ERR "copy_from_user failed\n");
    return -EFAULT;
    }

    if (count > sizeof(cbuf)) {
    printk(KERN_ERR "count exceeds buffer size\n");
    return -EINVAL;
    }

    for(int i=0;i<8;i++){
        printk(KERN_INFO "write for문 실행중입니다.");
        if(cbuf[i]==1){
            printk(KERN_INFO "cbuf[i]==1 일 때 실행중입니다.");
            gpio_set_value(GPIO4,1);
        }else{
            printk(KERN_INFO "cbuf[i]!=1 일 때 실행중입니다.");
            gpio_set_value(GPIO4,0);
        }

        gpio_set_value(GPIO23,1);
        mdelay(10);
        gpio_set_value(GPIO23,0);
        mdelay(10);
  
    }
    gpio_set_value(GPIO22,1);
    mdelay(10);
    gpio_set_value(GPIO22,0);
    mdelay(10);
    
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
        printk(KERN_ALERT "[%s] GPIO4 is not valid.\n",__func__);
        return -1;
    }

    if(gpio_is_valid(GPIO22)==false){
        printk(KERN_ALERT "[%s] GPIO22 is not valid.\n",__func__);
        return -1;
    }

    if(gpio_is_valid(GPIO23)==false){
        printk(KERN_ALERT "[%s] GPIO23 is not valid.\n",__func__);
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