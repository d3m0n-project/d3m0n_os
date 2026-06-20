#include "scripting.h"

t_fn_def	funcs[] = {
	{fn_log, {ARG_STR, ARG_STR, ARG_NULL}, "log"},
	{fn_alert, {ARG_STR, ARG_NULL}, "alert"},
	{
		.fn=template_load,
		.name="template.load",
		.args_type={ARG_STR, ARG_INT, ARG_INT, [3 ... (MAX_ARGS - 2)] = ARG_STR | ARG_OPTIONAL, ARG_NULL}
	},
	{fn_app_list, {ARG_NULL}, "app.list"},
	{fn_app_open, {ARG_STR, ARG_NULL}, "app.open"},
	{fn_app_exit, {ARG_NULL}, "app.exit"},
	{0}
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