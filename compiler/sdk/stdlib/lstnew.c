
#include "stdlib.h"

t_list	*lstnew(void *content)
{
	t_list	*current;

	current = malloc(sizeof(t_list));
	if (!current)
		return (0);
	current->content = content;
	current->next = 0;
	return (current);
}
