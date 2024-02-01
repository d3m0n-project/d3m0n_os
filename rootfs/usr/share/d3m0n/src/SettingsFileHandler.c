#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *getSetting(char *name, char *path)
{
    FILE *fptr = fopen(path, "r");
    
    if(fptr == NULL)
    {
		printf("[x] Error reading SettingsFile: doesn't exist\n");
        return "file not found";
    }
    
    
    // Store the content of the file
    char line[500];
	char *output = "not found";

    while (fgets(line, 500, fptr))
    {
        char *token = strtok(line, ": ");
        
		// printf("token: '%s', '%s'\n", token, name);
		
		if(strcmp(token, name) == 0) 
		{
			token = strtok(NULL, ": ");
			// printf("token: '%s'\n", token);

			output = strdup(token);
			break;
		}
        
    }

    // Close the file
    fclose(fptr);

    return(output);
}

void writeSetting(char *name, char *value, char *path)
{
	FILE *fptr;

	fptr = fopen(path,"w");

	if(fptr == NULL)
	{
		printf("[x] Error writting SettingsFile: doesn't exist\n");   
		exit(1);             
	}

	fprintf(fptr,"name2: custom_value");
	fclose(fptr);
}