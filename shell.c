#include "shell.h"

void print_prompt(char * pwd_path)
{
    printf("%s", pwd_path);
    printf("> ");
}
