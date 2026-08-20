
#include "stdlib.h"

t_list	*lstlast(t_list *lst)
{
	t_list	*current;

	current = lst;
	while (current)
	{
		if (current->next)
			current = current->next;
		else
			return (current);
	}
	return (0);
}
