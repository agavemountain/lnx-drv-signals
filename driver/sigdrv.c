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
#include <linux/ioctl.h>
#include <linux/signal.h>
#include <linux/poll.h>
#include <linux/aio.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joe Turner");
MODULE_DESCRIPTION("Posix Signal Driver Example");
MODULE_SUPPORTED_DEVICE("sigdrv");

#define DEVICE_NAME "sigdev"
#define SUCCESS 0
#define SIGDRV_IOC_MAGIC	100
#define IOCTL_PING  _IO(SIGDRV_IOC_MAGIC, 0)
//#define IOCTL_PING 9999

static int major;
static struct fasync_struct *async_queue;

// This is our vary own device structure where we can squirrel away
// our secret data until our hearts content.
struct sigdrv_dev {
//	struct cdev cdev;			// char device structure.
};

static int sigdrv_open(struct inode *inode, struct file *filp)
{
	struct sigdrv_dev *dev;  // device information

	// increment use count
	try_module_get(THIS_MODULE);


	printk("Opened.\n");
	return SUCCESS;
}

static int sigdrv_release(struct inode *inode, struct file *filp)
{
	// decrement use count
	module_put(THIS_MODULE);
	printk("Closed.\n");
	return SUCCESS;
}

static const char g_cheesy_string[] = "Sigdrv is alive.\n\0";
static const ssize_t cheesy_string_size = sizeof(g_cheesy_string);

// cat /dev/sigdrv to get the above string.
static ssize_t sigdrv_read( struct file *filep,
							char __user *user_buffer,
							size_t count,
							loff_t *position)
{
	struct sigdrv_dev *dev;  // device information

	printk( KERN_NOTICE "sigdrv: device read at offset %i, bytes %u", (int)*position,
			(unsigned int) count);

	dev = filep->private_data;

	// If we read past our string size, we are done.
	if (*position >= cheesy_string_size) {
		return 0;
	}

	// User is trying to read more bytes than we have, only give them
	// as many bytes as we have.
	if (*position + count > cheesy_string_size) {
		count = cheesy_string_size - *position;
	}

	if (copy_to_user(user_buffer, g_cheesy_string + *position, count) != 0) {
		return -EFAULT;
	}

	*position += count;	// move reading position
	return count;
}



static int sigdrv_fasync(int fd, struct file *filp, int on)
{
	printk(KERN_NOTICE "fasync, fd = %d, on = %d.\n", fd, on);
	return fasync_helper(fd, filp, on, &async_queue);
}

static int sigdrv_ioctl(struct inode *i, struct file *filp, unsigned int cmd, unsigned long arg)
{

	switch (cmd)
	{
	case IOCTL_PING:
		printk(KERN_NOTICE "Received ioctl command.\n");
		kill_fasync(&async_queue, SIGIO, POLL_IN);
		break;

	default:
		printk(KERN_NOTICE "Recieved invalid ioctl: %d\n", cmd);
		// The POSIX standard states that if an inappropriate ioctl command
		// has been issued, then -ENOTTY is returned.
		return -ENOTTY;
	}

	return 0;
}

// File operations structure.
static struct file_operations fops =
{
	.owner = THIS_MODULE,
	.open = sigdrv_open,
	.read = sigdrv_read,
	.release = sigdrv_release,
	.fasync = sigdrv_fasync,
	.ioctl = sigdrv_ioctl,
};

static int __init init_sigdrv(void)
{
	int err = -ENOMEM;

	// register device, should show up in /proc/devices
	major = register_chrdev(0, DEVICE_NAME, &fops);
	if (major < 0)
	{
		printk(KERN_ALERT "Failed to register device with %d\n", major);
		return major;
	}
    printk( KERN_INFO "sigdrv driver loaded, major = %d.\n", major);
    return 0;  // success.

  fail:
    return err;
}

static __exit void cleanup_sigdrv(void)
{
	// unregister device, should be removed from /proc/devices
    unregister_chrdev(major, DEVICE_NAME);
    printk( KERN_INFO "sigdrv unloaded.\n");
}

module_init(init_sigdrv);
module_exit(cleanup_sigdrv);

