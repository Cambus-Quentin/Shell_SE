#ifndef __SHELL__H__
#define __SHELL__H__

#include <stdio.h>
#define CYAN "\033[0;36m"

void print_prompt(char *pwd_path);

void run_shell(int pwd_loc, int path_loc, char **envp);

#endif