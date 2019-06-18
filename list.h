#ifndef LIST_H
#define LIST_H

typedef struct s_keylst t_keylst;

int		browse_linked_list(t_keylst *lst, char *buffer, loff_t **offset, size_t len);
int		add_to_list(unsigned char scancode);

#endif
