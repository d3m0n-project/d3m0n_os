#include <sys/mman.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#ifndef INCLUDE_SDK
	#define INCLUDE_SDK	1
#endif

#ifndef LIB_PATH
	#define LIB_PATH "None"
#endif

#if INCLUDE_SDK == 1
extern "C"
{
	extern const unsigned char _binary_obj_sdk_a_start[];
	extern const unsigned char _binary_obj_sdk_a_end[];

	extern const unsigned char _binary_linker_ld_start[];
	extern const unsigned char _binary_linker_ld_end[];
}

static int	create_embedded_archive(const char *name, const unsigned char *begin, const unsigned char *end)
{
	const size_t size = static_cast<size_t>(end - begin);
	int fd = memfd_create(name, 0);
	if (fd == -1)
	{
		perror("memfd_create");
		return -1;
	}

	const unsigned char *ptr = begin;
	size_t remaining = size;
	while (remaining > 0)
	{
		ssize_t written = write(fd, ptr, remaining);
		if (written == -1)
		{
			if (errno == EINTR)
				continue;

			perror("write");
			close(fd);
			return -1;
		}

		ptr += written;
		remaining -= static_cast<size_t>(written);
	}
	return fd;
}
#endif

int main(int argc, char **argv)
{
	if (argc == 1)
	{
		cerr << "\033[31mERROR\033[0m: Usage: " << argv[0] << " [options] file..." << endl;
		return 1;
	}

	bool compile_only = false;
	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
		{
			cout << "d3c: the official d3m0n os compiler" << endl;
			cout << "\033[30m" << "	SDK LOADED:     " << "\033[0m" << (INCLUDE_SDK?"\033[32mYes":"\033[31mNo") << "\033[0m" << endl;
			cout << "\033[30m" << "	SDK PATH:       " << "\033[0m" << LIB_PATH << endl;
			cout << "\033[30m" << "	AUTHOR:         " << "\033[0m" << "4re5 group" << "\033[0m" << endl;
			return 0;
		}
		if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "-E") == 0 || strcmp(argv[i], "-S") == 0)
		{
			compile_only = true;
			break;
		}
	}

	#if INCLUDE_SDK == 1
	int fd_sdk_a = -1;
	int fd_linker = -1;
	if (!compile_only)
	{
		fd_sdk_a = create_embedded_archive("d3m0n_sdk.a", _binary_obj_sdk_a_start, _binary_obj_sdk_a_end);
		if (fd_sdk_a == -1)
		{
			cerr << "\033[31mERROR\033[0m: Could not allocate d3m0n OS SDK" << endl;
			return 1;
		}
		fd_linker = create_embedded_archive("linker.ld", _binary_linker_ld_start, _binary_linker_ld_end);
		if (fd_linker == -1)
		{
			close(fd_sdk_a);
			cerr << "\033[31mERROR\033[0m: Could not allocate d3m0n OS linker script" << endl;
			return 1;
		}
	}
	#endif

	vector<string> arguments;
	arguments.emplace_back("arm-none-eabi-g++");
	arguments.emplace_back("-ffreestanding");
	arguments.emplace_back("-nostdlib");
	arguments.emplace_back("-mcpu=arm1176jzf-s");
	#if INCLUDE_SDK == 1
	arguments.emplace_back("-I" + (string)LIB_PATH);
	#endif
	if (!compile_only)
		arguments.emplace_back("-Wl,-e,_start");

	for (int i = 1; i < argc; ++i)
		arguments.emplace_back(argv[i]);

	
	if (!compile_only)
	{
		int use_group = 0;
		
		#if INCLUDE_SDK == 1
		arguments.emplace_back("-Wl,--start-group");
		const string sdk_archive = "/proc/self/fd/" + to_string(fd_sdk_a);
		arguments.emplace_back(sdk_archive);
		use_group = 1;
		#endif
		
		arguments.emplace_back("-lgcc");
		if (use_group)
			arguments.emplace_back("-Wl,--end-group");
		
		#if INCLUDE_SDK == 1
		const string linker_archive = "/proc/self/fd/" + to_string(fd_linker);
		arguments.emplace_back("-T");
		arguments.emplace_back(linker_archive);
		#endif
	}
	

	vector<char *> exec_args;
	for (string &arg : arguments)
		exec_args.push_back(arg.data());
	exec_args.push_back(nullptr);


	cout << "\033[36m";
	for (char **p = exec_args.data(); *p; ++p)
		cout << *p << ' ';
	cout << "\033[0m\n";
	execvp("arm-none-eabi-gcc", exec_args.data());
	perror("execvp");

	#if INCLUDE_SDK == 1
	if (fd_linker != -1)
		close(fd_linker);
	if (fd_sdk_a)
		close(fd_sdk_a);
	#endif

	return 1;
}