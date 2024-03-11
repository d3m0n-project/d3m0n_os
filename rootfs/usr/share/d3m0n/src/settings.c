#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "settings.h"


char* delete_crlf(const char* input) {
    size_t input_length = strlen(input);
    char* result = (char*)malloc(input_length + 1); // Allocate memory for the result
    if (result == NULL) {
        return NULL; // Memory allocation failed
    }

    size_t j = 0; // Index for the result buffer
    for (size_t i = 0; i < input_length; ++i) {
        // Look for "\r\n" and skip them
        if (input[i] == '\r' && i + 1 < input_length && input[i + 1] == '\n') {
            ++i; // Skip the '\n' too
            continue;
        }

        // Copy character to the result buffer
        result[j++] = input[i];
    }
    result[j] = '\0'; // Null-terminate the result string

    return result;
}

char* getSetting(char* name, char* path)
{
    FILE* fptr = fopen(path, "r");
    
    if(fptr == NULL)
    {
		printf("[x] Error reading SettingsFile: doesn't exist");
        return "file not found";
    }
    
    
    // Store the content of the file
    char line[500];
	char* output = "not found";

    while (fgets(line, 500, fptr))
    {
        char* token = strtok(line, ": ");
        
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

    return(delete_crlf(output));
}

void writeSetting(char* name, char* value, char* path)
{
	FILE* fptr;

	fptr = fopen(path,"w");

	if(fptr == NULL)
	{
		printf("[x] Error writting SettingsFile: doesn't exist\n");   
		exit(1);             
	}

	fprintf(fptr,"name2: custom_value");
	fclose(fptr);
}