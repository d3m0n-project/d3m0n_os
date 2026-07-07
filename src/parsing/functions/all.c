#include "scripting.h"

t_fn_def	funcs[] = {
	{fn_log, {ARG_STR, ARG_STR, ARG_NULL}, "log", 0},
	{fn_alert, {ARG_STR, ARG_NULL}, "alert", 0},
	{
		.fn=template_load,
		.name="template.load",
		.args_type={ARG_STR, ARG_INT, ARG_INT, [3 ... (MAX_ARGS - 2)] = ARG_STR | ARG_OPTIONAL, ARG_NULL},
		.is_return=0
	},
	{fn_app_list, {ARG_NULL}, "app.list", 0}, // special case
	{fn_app_open, {ARG_STR, ARG_NULL}, "app.open", 0},
	{fn_app_exit, {ARG_NULL}, "app.exit", 0},
	{fn_settings_set, {ARG_STR, ARG_STR, ARG_NULL}, "settings.set", 0},
	{fn_settings_get, {ARG_STR, ARG_NULL}, "settings.get", 1},
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