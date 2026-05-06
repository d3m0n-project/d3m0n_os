#include "scripting.h"

t_fn_def	funcs[FUNCS_SIZE] = {
	{
		.fn=fn_log,
		.name="log",
		.args_type={ARG_STR, ARG_STR, ARG_NULL}
	},
	{
		.fn=fn_alert,
		.name="alert",
		.args_type={ARG_STR, ARG_NULL}
	},
	{
		.fn=template_load,
		.name="template.load",
		.args_type={ARG_STR, ARG_INT, ARG_INT, [3 ... (MAX_ARGS - 2)] = ARG_STR | ARG_OPTIONAL, ARG_NULL}
	}
};





char	*get_arg_type_name(e_arg_type type)
{
	switch (type)
	{
	case ARG_NULL:
		return "Nothing";
	case ARG_STR:
		return "STR";
	case ARG_INT:
		return "INT";
	default:
		return "Unknown";
	}
}