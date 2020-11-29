#include "shell.h"

void print_prompt(char *pwd_path)
{
    printf(CYAN "%s" RESET, pwd_path);
    printf("> ");
}
