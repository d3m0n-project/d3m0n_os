#include "app.hpp"
#include <ctype.h>

class App
{
public:
	string	name;
	string	package;
	uint8_t	*icon;
int	parse_manifest(const string& path);
//int	parse_manifest(const string& path);
};