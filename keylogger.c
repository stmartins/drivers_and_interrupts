#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/fs.h>

#define PATH "keylog"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("stmartin");
MODULE_DESCRIPTION("Module keylog");

static ssize_t	key_write(struct file *filep, const char *buffer, size_t len, loff_t *offset);
struct dentry	*dir_entry, *file_entry;


static ssize_t	key_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	printk(KERN_INFO "in key_write\n");
	return 1;
}

static struct file_operations	key_ops = {

	.owner = THIS_MODULE,
	.write = key_write,
};


int		create_dir(void)
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

int		create_file(const char *name, umode_t mode, struct file_operations *fops)
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

static int	__init keylogger_init(void)
{
	printk(KERN_INFO "in keylogger init function \n");
	if (create_dir() != 1)
	{
		printk(KERN_INFO "creation directory failed\n");
		return 0;
	}
	if (create_file("keylogs", 0660, &key_ops))
	{
		printk(KERN_INFO "file creation failed \n");
		return 0;
	}
	return 1;
}

static void	__exit keylogger_exit(void)
{
	printk(KERN_INFO "exit keylogger\n");
	debugfs_remove_recursive(dir_entry);
}

module_init(keylogger_init);
module_exit(keylogger_exit);
