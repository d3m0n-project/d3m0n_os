#include "utils.hpp"
#include "app.hpp"

using namespace std;

int	main(int argc, char **argv)
{
	cout << "--------------------------------" << endl;
	cout << "      d3m0n os - C Compiler      " << endl;
	cout << "              v1.0              " << endl;
	cout << "--------------------------------" << endl;
	//if (argc == 1)
	//{
	//	cerr << "\033[31mERROR\033[0m: Usage: " << argv[0] << " path/to/app_dir/" << endl;
	//	return 1;
	//}

	string base_command = "arm-none-eabi-gcc -Wall -Wextra -Werror -ffreestanding -nostdlib -O2 -Isdk/lib -mcpu=arm1176jzf-s -lgcc -Wl,-e,_start sdk/*.c ";
	for (int i=1; i<argc; i++)
	{
		base_command += argv[i];
		base_command += " ";
	}
	int ret = system(base_command.c_str());
	if (ret != 0)
	{
		cerr << "\033[31mERROR\033[0m: Could not compile d3m0n C code" << endl;
		return 1;
	}

	cout << "\033[32mSUCCESS\033[0m: Successfully compiled d3m0n C code!" << endl;
	//char	*path = argv[1];
	//if (!file_exists((const string)path))
	//{
	//	cerr << "\033[31mERROR\033[0m: Could not find app directory to build: " << path << endl;
	//	return 1;
	//}

	//App new_app();
	//new_app
	//new_app::parse_

	return 0;
}