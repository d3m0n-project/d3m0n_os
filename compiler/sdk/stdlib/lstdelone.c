
#include "stdlib.h"

void	lstdelone(t_list *lst, void (*del)(void *))
{
	del(lst->content);
	free(lst);
}
