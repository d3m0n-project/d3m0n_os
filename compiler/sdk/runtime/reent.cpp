extern "C"
{
	struct _reent
	{
		int _errno;
	};

	static _reent g_reent = {};

	_reent *_impure_ptr = &g_reent;
}