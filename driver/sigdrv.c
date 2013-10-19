/*
    sigdrv.cpp

    Copyright (C) 2013 Joe Turner <joe@agavemountain.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joe Turner");
MODULE_DESCRIPTION("Posix Signal Driver");
MODULE_SUPPORTED_DEVICE("sigdrv");

#define DEVICE_NAME "sigdev"
#define SUCCESS 0

static int major;
static int device_open = 0;

static int sigdrv_open(struct inode *inode, struct file *filp)
{
	try_module_get(THIS_MODULE);
	return SUCCESS;
}

static int sigdrv_release(struct inode *inode, struct file *filp)
{
	module_put(THIS_MODULE);
	return SUCCESS;
}

static struct file_operations fops =
{
	.open = sigdrv_open,
	.release = sigdrv_release
};

static int __init init_sigdrv(void)
{
	major = register_chrdev(0, DEVICE_NAME, &fops);
	if (major < 0)
	{
		printk(KERN_ALERT "Failed to register device with %d\n", major);
		return major;
	}
    printk( KERN_INFO "sigdrv driver loaded.\n");
    return 0;
}

static __exit void cleanup_sigdrv(void)
{
    unregister_chrdev(major, DEVICE_NAME);
    printk( KERN_INFO "sigdrv unloaded.\n");
}

module_init(init_sigdrv);
module_exit(cleanup_sigdrv);

