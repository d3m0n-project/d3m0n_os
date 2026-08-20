
#include "stdlib.h"

void	lstadd_front(t_list **lst, t_list *new)
{
	t_list	*old_first_node;

	old_first_node = *lst;
	new->next = old_first_node;
	*lst = new;
}
