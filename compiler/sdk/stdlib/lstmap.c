
#include "stdlib.h"

t_list	*lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*new_lst;
	t_list	*new_elem;
	void	*new_content;

	if (!lst || !f)
		return (0);
	new_lst = 0;
	while (lst)
	{
		new_content = f(lst->content);
		new_elem = lstnew(new_content);
		if (!new_elem)
		{
			if (del)
				del(new_content);
			lstclear(&new_lst, del);
			return (0);
		}
		lstadd_back(&new_lst, new_elem);
		lst = lst->next;
	}
	return (new_lst);
}
