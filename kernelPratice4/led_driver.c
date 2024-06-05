#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>


#define GPIO4 (4)
#define GPIO22 (22)

static dev_t my_dev;
static struct cdev my_cdev;

unsigned int leed_state=0;
int irq_num=0;
int irqNum[2];

int device_open(struct inode* inode,struct file* filp);
int device_release(struct inode* inode, struct file* filp);
ssize_t device_read(struct file* filp, const char* buf,size_t count,loff_t* f_pos);
ssize_t device_write(struct file* filp, const char* buf, size_t count,loff_t* f_pos);

static struct file_operations fops={
    .read=device_read,
    .write=device_write,
    .open=device_open,
    .release=device_release,
};

static irqreturn_t switch_irq_handler(int irq,void* dev_id){

    irq_num=irq;

    static unsigned long flags=0;

    local_irq_save(flags);

    gpio_set_value(GPIO4,led_state);

    local_irq_restore(flags);

    return IRQ_HANDLED;
}

int device_open(struct inode* inode, struct file* filp){
    int err;
    int id=GPIO4;
    err=gpio_request(id,"LED");
    if(err){
        printk("LED failed to request GPIO %d\n",id);
    }

    gpio_direction_output(GPIO4,0);

    id=GPIO22;
    err=gpio_request(id,"Switch");
    if(err){
        printk("Switch failed to set debounce GPIO %d\n",id);
    }
    gpio_direction_input(GPIO22);

    irqNum[0]=gpio_to_irq(GPIPO22,200);
    err=request_irq(irqNum[0],(void*)switch_irq_handler,IRQF_TRIGGER_RISIng,"my_led_dev",NULL);
    if(err){
        printk("Switch failed to request irq %d\n",id);
    }

    printk(KERN_INFO "GPIO LED & switch open\n");

    return 0;
}

int device_release(struct inode* inode, struct file* filp){

    printk(KERN_INFO "GPIO LED release\n");
    free_irq(irqNum[0],NULL);
    gpio_free(GPIO4);
    gpio_free(GPIO22);
    return 0;
}

ssize_t device_write(struct file* filp,const char* buf,size_t count,loff_t* f_pos){
    unsigned char cbuf[10]={0,};
    copy_from_user(cbuf,buf,count);

    printk("Write(): GPIO4 To %d\n",cbuf[0]);

    if(cbuf[0]==1){
        gpio_set_value(GPIO4,1);
    }else{
        gpio_set_value(GPIO4,0);
    }

    return count;
}

ssize_t device_read(struct file* filp,char* buf,size_t count,loff_t* f_pos){
    unsigned char switch_state=0;
    switch_state=gpio_get_value(GPIO22);
    copy_to_user(buf,&switch_state,1);

    printk("Read(): GPIO22 To %d\n",switch_state);

    return count;
}

int device_init(void){
    if(alloc_chrdev_region(&my_dev,0,1,"my_led_dev"){
        printk(KERN_ALERT "[%s] alloc_chrdev_region failed\n",__func__);
        return -1;
    })
    
    printk("LED Major =%d, Minor=%d\n",MAJOR(my_dev),MINOR(my_dev));

    cdev_init(&my_cdev,&fops);
    if(cdev_add(&my_cdev,my_dev,1)){
        printk(KERN_ALERT "[%s] cdev_add failed\n",__func__);
        return -1;
    }

    if(gpio_is_valid(GPIO4)==false){
        printk(KERN_ALERT "[%s] GPIO is not valid\n ",__func__);
        return -1;
    }

    if(gpio_is_valid(GPIO22)==false){
        printk(KERN_ALERT "[%s] GPIO is not valid\n",__func__);
        return -1;
    }

    return 0;
}

void device_exit(void){
    cdev_del(&my_cdev);
    unregister_chrdev_region(my_dev,1);
}

module_init(device_init);
module_exit(device_exit);
MODULE_LICENSE("GPL");
