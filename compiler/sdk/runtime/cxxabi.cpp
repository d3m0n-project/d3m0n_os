extern "C" {
	void* __dso_handle = nullptr;

	int __cxa_atexit(void (*destructor)(void*), void* object, void* dso_handle)
	{
		(void)destructor;
		(void)object;
		(void)dso_handle;
		return 0;
	}

	void	__cxa_finalize(void *)
	{
		return;
	}

	void	__cxa_pure_virtual()
	{
		while (1)
			;
	}

	void	__cxa_deleted_virtual()
	{
		while (1)
			;
	}
}