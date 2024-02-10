#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *getSetting(char *name, char *path)
{
    FILE *fptr = fopen(path, "r");
    
    if(fptr == NULL)
    {
        return "file not found";
    }
    
    
    // Store the content of the file
    char line[500];
    char *output="not found";

    // Read the content and store it inside myString
    while (fgets(line, 500, fptr))
    {
        printf("%s", line);

        char *token = strtok(line, ": ");
        

        while (token != NULL)
        {
            printf("%s\n", token);
            token = strtok(NULL, ": ");
        }
    }

    // Close the file
    fclose(fptr);

    return output;
}