#ifndef PACKAGE_MANAGER_H
#define PACKAGE_MANAGER_H

typedef enum
{
	PACKAGE_PATH,
	PACKAGE_MANIFEST,
	PACKAGE_MAIN_SOURCE,
	PACKAGE_MAIN_LAYOUT,
	PACKAGE_LAYOUT_PATH,
	PACKAGE_SOURCE_PATH
}	e_package_request_type;

char	*get_app_path_from_package(char *package, e_package_request_type type);
int		load_app_list(void);
char	*get_package_from_manifest_path(char *path);

void	cleanup_splitted(char **splitted);

#endif