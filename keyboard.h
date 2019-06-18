#ifndef KEYBOARD_MAP_H
#define KEYBOARD_MAP_H

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

static const unsigned char	*keyboard_name[128] = {
	0,				// undefined
	"escape",		// escape
	"1","2","3","4","5","6","7","8","9","0","-","=",
	"backspace",	// backspace
	"tab",			// tab
	"q","w","e","r","t","y","u","i","o","p","[","]","enter",
	"left ctrl",	// Ctrl
	"a","s","d","f","g","h","j","k","l",";","\"","`",
	"left shift",	// Left shift
	"\\","z","x","c","v","b","n","m",",",".","/",
	"right shift",	// Right shift
	"*",			// (Keypad *)
	"left alt",		// left alt
	" ",
	"CapsLock",		// Caps lock
	"F1",			// <F1>
	"F2",			// <F2>
	"F3",			// <F3>
	"F4",			// <F4>
	"F5",			// <F5>
	"F6",			// <F6>
	"F7",			// <F7>
	"F8",			// <F8>
	"F9",			// <F9>
	"F10",			// <F10>
	"NumberLock",	// Num lock
	"ScrollLock",	// Scroll Lock
	"Home",			// Home key
	"cursor up",	// Up Arrow
	"page up",		// Page Up
	"-",
	"cursor left",	// Left Arrow
	"",
	"cursor right",	// Right Arrow
	"+",
	"end",			// End key
	"cursor down",	// Down Arrow
	"page down",	// Page Down
	"insert",		// Insert Key
	"delete",		// Delete Key
	"", "", "",		// ?
	"F11",			// <F11>
	"F12",			// <F12>
	"", "",			// ?
	"super",
	"","","","","","","","","","","","","","","","","","","","","","","","","",
	"","","","","","","","","","",""
};

#endif
