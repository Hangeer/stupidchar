#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
/**
 *  module.h => 模块所需要的大量符号和函数定义
 *  init.h => 指定初始化和清除函数
 *  fs.h => 文件系统相关
 *  cdev.h => cdev 结构的头文件
 *  uaccess.h => 在内核和用户空间移动数据的函数
 *  slab.h => 提供 kfree kmalloc (高版本的内核需要手动引入，不能隐式调用)
 */

#define C_SIZE (sizeof(char) * 20)
//  存储字符大小

MODULE_LICENSE("GPL");
//  开源许可证

int stupidchar_open(struct inode *, struct file *);
int stupidchar_release(struct inode *, struct file *);
ssize_t stupidchar_read(struct file *, char *, size_t, loff_t *);
ssize_t stupidchar_write(struct file *, const char *, size_t, loff_t *);

int dev_major = 50;
int dev_minor = 0;
/**
 *  主设备号和次设备号
 *  讲道理与 mknod 命令所需参数对应，但是实际运行的时候 dev_major 返回249
 */

struct file_operations stupidchar_fops = {
    owner: THIS_MODULE,
    open: stupidchar_open,
    release: stupidchar_release,
    read: stupidchar_read,
    write: stupidchar_write,
};
/**
 *  文件操作与分配的设备号相连
 */

struct stupidchar_dev {
    char stupid_char[C_SIZE];
    struct cdev cdev;
};
/**
 *  自定义设备结构
 *  stupid_char => 将要操作的设备
 *  cdev => 内核中表示字符设备的结构
 */

struct stupidchar_dev *my_dev;
//  设备结构的指针

static void __exit stupidchar_exit(void) {
    printk("invoke stupidchar_exit\n");
    
    dev_t devno = MKDEV(dev_major, dev_minor);
    cdev_del(&my_dev -> cdev);
    kfree(my_dev);
    unregister_chrdev_region(devno, 1);
    printk("stupidchar unregister success\n");
}
/**
 *  退出模块时候相关操作
 *  devno => 表示设备编号的结构
 *  cdev_del => 从系统中移除一个字符设备
 *  kfree => 释放自定义设备结构
 *  unregister_chrdev_region => 注销已经注册的驱动程序
 */

static int __init stupidchar_init(void) {
    printk("invoke stupidchar_init\n");
    
    int ret, err;
    dev_t devno = MKDEV(dev_major, dev_minor);
    ret = alloc_chrdev_region(&devno, dev_minor, 1, "stupidchar");
    dev_major = MAJOR(devno);
    /**
     *  devno => 动态分配设备号，次设备号已经绑定
     *  ret => 保存动态分配的主设备号
     *  dev_major => 根据期望值分配设备号
     */

    if (ret < 0) {
        printk("stupidchar register failure\n");
        stupidchar_exit();
        return ret;
    } else {
        printk("stupidchar register success\n");
    }

    my_dev = kmalloc(sizeof(struct stupidchar_dev), GFP_KERNEL);

    if(!my_dev) {
        ret = -ENOMEM;
        printk("create device failed\n");
    } else {
        strcpy((my_dev -> stupid_char), "DEFAULT STR");
        cdev_init(&my_dev->cdev, &stupidchar_fops);
        my_dev -> cdev.owner = THIS_MODULE;
        err = cdev_add(&my_dev -> cdev, devno, 1);
        /**
         *  my_dev -> stupid_char => 设备变量 初始化为 0 
         *  cdev_init => 初始化设备中的 cdev 结构
         *  my_dev -> cdev.owner => 初始化 cdev 中的所有者字段
         *  err => 想内核添加这个 cdev 结构
         */
        if(err < 0) {
            printk("add device failure\n");
        }
    }
    return ret;
}
/**
 *  初始化模块的操作
 *  
 */

int stupidchar_open(struct inode *inode, struct file *filp)  {
    printk("invoke stupidchar_open\n");
    struct stupidchar_dev *dev;
    dev = container_of(inode -> i_cdev, struct stupidchar_dev, cdev);
    filp -> private_data = dev;
    return 0;
}
/**
 *  打开设备文件系统调用对应的操作
 *  根据 inode 结构的 cdev 字段，获得整个设备结构的指针
 *  将 file 结构中的 private_data 字段指向已分配的设备结构
 */

int stupidchar_release(struct inode *inode, struct file *filp) {
    printk("invoke stupidchar_release\n");
    return 0;
}
//  关闭设备文件系统调用对应操作

ssize_t stupidchar_read(struct file *filp, char *buf, size_t len, loff_t *off)  {
    printk("invoke stupidchar_read\n");
    struct stupidchar_dev *dev = filp -> private_data;
    if(copy_to_user(buf, &dev -> stupid_char, C_SIZE)) {
        return -EFAULT;
    }
    return C_SIZE;
}
//  读设备文件系统调用对应操作

ssize_t stupidchar_write(struct file *filp, const char *buf, size_t len, loff_t *off) {
    printk("invoke stupidchar_write\n");
    struct stupidchar_dev *dev = filp -> private_data;
    if(copy_from_user(&dev -> stupid_char, buf, C_SIZE)) {
        return -EFAULT;
    }
    return C_SIZE;
}
//  写设备文件系统调用对应操作

module_init(stupidchar_init);
module_exit(stupidchar_exit);
/**
 *  模块被装载时调用globalvar_init
 *  模块被卸载时调用globalvar_exit
 */