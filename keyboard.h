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

unsigned char keyboard_map[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

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
