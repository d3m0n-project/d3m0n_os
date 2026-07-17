#include "scripting.h"
#include "libft.h"

char	*fn_str_len(void **args)
{
	char	*text = ((char **)args)[0];
	return ft_itoa(ft_strlen(text));
}

char	*fn_str_is_empty(void **args)
{
	char	*text = ((char **)args)[0];
	return (ft_strlen(text) > 0)?"0":"1";
}
char	*fn_str_is_digit(void **args)
{
	char	*text = ((char **)args)[0];
	size_t	len = ft_strlen(text);
	for(size_t i=0; i<len; i++)
	{
		if (!ft_isdigit(text[i]))
			return "0";
	}
	return "1";
}
char	*fn_str_is_alpha(void **args)
{
	char	*text = ((char **)args)[0];
	size_t	len = ft_strlen(text);
	for(size_t i=0; i<len; i++)
	{
		if (!ft_isalpha(text[i]))
			return "0";
	}
	return "1";
}
char	*fn_str_is_alnum(void **args)
{
	char	*text = ((char **)args)[0];
	size_t	len = ft_strlen(text);
	for(size_t i=0; i<len; i++)
	{
		if (!ft_isalnum(text[i]))
			return "0";
	}
	return "1";
}
char	*fn_str_is_space(void **args)
{
	char	*text = ((char **)args)[0];

	if (*text == '\0')
		return "0";

	while (*text)
	{
		if (!ft_isspace(*text))
			return "0";
		text++;
	}
	return "1";
}

char	*fn_str_replace(void **args)
{
	char	*text = ((char **)args)[0];
	char	*old = ((char **)args)[1];
	char	*new = ((char **)args)[2];

	char	*result;
	char	*pos;
	size_t	old_len;
	size_t	new_len;
	size_t	count;
	size_t	len;

	old_len = ft_strlen(old);
	new_len = ft_strlen(new);
	if (old_len == 0)
		return ft_strdup(text);

	count = 0;
	pos = text;
	while ((pos = ft_strnstr(pos, old, ft_strlen(pos))))
	{
		count++;
		pos += old_len;
	}
	len = ft_strlen(text) + count * (new_len - old_len);
	result = malloc(len + 1);
	if (!result)
		return 0;

	result[0] = '\0';
	while ((pos = ft_strnstr(text, old, ft_strlen(text))))
	{
		ft_strncat(result, text, pos - text);
		ft_strcat(result, new);

		text = pos + old_len;
	}
	ft_strcat(result, text);
	return result;
}


char	*fn_str_join(void **args)
{
	char	*text1 = ((char **)args)[0];
	char	*text2 = ((char **)args)[1];

	return ft_strjoin(text1, text2);
}

char	*fn_str_cmp(void **args)
{
	char	*text1 = ((char **)args)[0];
	char	*text2 = ((char **)args)[1];

	return ft_itoa(ft_strcmp(text1, text2));
}

char	*fn_str_equal(void **args)
{
	char	*text1 = ((char **)args)[0];
	char	*text2 = ((char **)args)[1];

	return (ft_strcmp(text1, text2)?"0":"1");
}

char	*fn_str_substr(void **args)
{
	char	*text = ((char **)args)[0];
	int		start = *((int **)args)[1];
	int		len =   *((int **)args)[2];

	return ft_substr(text, start, len);
}

char	*fn_str_starts_with(void **args)
{
	char	*text = ((char **)args)[0];
	char	*prefix = ((char **)args)[1];
	size_t	len = ft_strlen(prefix);

	if (ft_strncmp(text, prefix, len) == 0)
		return "1";
	return "0";
}

char	*fn_str_ends_with(void **args)
{
	char	*text = ((char **)args)[0];
	char	*suffix = ((char **)args)[1];

	size_t	text_len = ft_strlen(text);
	size_t	suffix_len = ft_strlen(suffix);

	if (suffix_len > text_len)
		return "0";

	if (ft_strncmp(text + text_len - suffix_len, suffix, suffix_len) == 0)
		return "1";

	return "0";
}

char	*fn_str_contains(void **args)
{
	char	*text = ((char **)args)[0];
	char	*search = ((char **)args)[1];

	return (ft_strnstr(text, search, ft_strlen(text))) ? "1" : "0";
}


char	*fn_str_lower(void **args)
{
	char	*text = ((char **)args)[0];
	size_t	len = ft_strlen(text);
	char	*dup = ft_strdup(text);

	if (!dup)
		return 0;
	
	for (size_t i=0; i<len; i++)
		dup[i] = ft_tolower(dup[i]);
	return dup;
}
char	*fn_str_upper(void **args)
{
	char	*text = ((char **)args)[0];
	size_t	len = ft_strlen(text);
	char	*dup = ft_strdup(text);

	if (!dup)
		return 0;
	
	for (size_t i=0; i<len; i++)
		dup[i] = ft_toupper(dup[i]);
	return dup;
}

char	*fn_str_trim_left(void **args)
{
	char	*text = ((char **)args)[0];

	while (*text && ft_isspace(*text))
		text++;

	return ft_strdup(text);
}


char	*fn_str_trim_right(void **args)
{
	char	*text = ((char **)args)[0];
	char	*dup;
	int		len;

	len = ft_strlen(text);

	while (len > 0 && ft_isspace(text[len - 1]))
		len--;

	dup = malloc(len + 1);
	if (!dup)
		return 0;

	ft_memcpy(dup, text, len);
	dup[len] = '\0';

	return dup;
}


char	*fn_str_trim(void **args)
{
	char	*text = ((char **)args)[0];
	char	*start;
	char	*result;
	int		len;

	start = text;

	while (*start && ft_isspace(*start))
		start++;

	len = ft_strlen(start);

	while (len > 0 && ft_isspace(start[len - 1]))
		len--;

	result = malloc(len + 1);
	if (!result)
		return 0;

	ft_memcpy(result, start, len);
	result[len] = '\0';

	return result;
}


char	*fn_str_reverse(void **args)
{
	char	*text = ((char **)args)[0];
	size_t	len = ft_strlen(text);
	char	*dup = ft_strdup(text);

	if (!dup)
		return 0;
	
	for (size_t i=0; i<len / 2; i++)
	{
		char	tmp = dup[i];
		dup[i] = dup[len - 1 - i];
		dup[len - 1 - i] = tmp;
	}
	return dup;
}