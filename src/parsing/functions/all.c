#include "scripting.h"

t_fn_def	funcs[FUNCS_SIZE] = {
	{
		.fn=fn_log,
		.name="log"
	},
	{
		.fn=fn_alert,
		.name="alert"
	}
};