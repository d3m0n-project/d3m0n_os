
#include "stdlib.h"

void	lstadd_back(t_list **lst, t_list *new)
{
	t_list	*current;

	current = *lst;
	if (!current)
	{
		*lst = new;
		return ;
	}
	while (current)
	{
		if (current->next)
			current = current->next;
		else
		{
			current->next = new;
			break ;
		}
	}
}
