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
	{fn_window_open, {ARG_STR, ARG_NULL}, "window.open", 0},
	{fn_window_go_back, {ARG_NULL}, "window.goback", 0},
	{fn_state_set, {ARG_STR, ARG_STR, ARG_STR, ARG_NULL}, "state.set", 0},
	{fn_state_get, {ARG_STR, ARG_STR, ARG_NULL}, "state.get", 1},
	{fn_system_var_get, {ARG_STR, ARG_NULL}, "system.get", 1},

	{fn_str_len, {ARG_STR, ARG_NULL}, "string.len", 1},
	{fn_str_is_empty, {ARG_STR, ARG_NULL}, "string.is_empty", 1},
	{fn_str_is_digit, {ARG_STR, ARG_NULL}, "string.is_digit", 1},
	{fn_str_is_alpha, {ARG_STR, ARG_NULL}, "string.is_alpha", 1},
	{fn_str_is_alnum, {ARG_STR, ARG_NULL}, "string.is_alphanum", 1},
	{fn_str_is_space, {ARG_STR, ARG_NULL}, "string.is_space", 1},
	{fn_str_lower, {ARG_STR, ARG_NULL}, "string.lower", 1},
	{fn_str_upper, {ARG_STR, ARG_NULL}, "string.upper", 1},

	{fn_str_substr, {ARG_STR, ARG_STR, ARG_NULL}, "string.substr", 1},
	{fn_str_join, {ARG_STR, ARG_STR, ARG_NULL}, "string.join", 1},
	{fn_str_cmp, {ARG_STR, ARG_STR, ARG_NULL}, "string.cmp", 1},
	{fn_str_equal, {ARG_STR, ARG_STR, ARG_NULL}, "string.equal", 1},
	{fn_str_starts_with, {ARG_STR, ARG_STR, ARG_NULL}, "string.starts_with", 1},
	{fn_str_ends_with, {ARG_STR, ARG_STR, ARG_NULL}, "string.ends_with", 1},
	{fn_str_contains, {ARG_STR, ARG_STR, ARG_NULL}, "string.contains", 1},
	{fn_str_trim, {ARG_STR, ARG_NULL}, "string.trim", 1},
	{fn_str_trim_left, {ARG_STR, ARG_NULL}, "string.trim_left", 1},
	{fn_str_trim_right, {ARG_STR, ARG_NULL}, "string.trim_right", 1},
	{fn_str_replace, {ARG_STR, ARG_STR, ARG_NULL}, "string.replace", 1},
	{fn_str_reverse, {ARG_STR, ARG_NULL}, "string.reverse", 1},
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