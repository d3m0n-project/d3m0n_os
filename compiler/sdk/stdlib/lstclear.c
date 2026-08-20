
#include "stdlib.h"

void	lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*current;
	t_list	*next_one;

	current = *lst;
	while (current)
	{
		next_one = current->next;
		lstdelone(current, del);
		current = next_one;
	}
	*lst = 0;
}
