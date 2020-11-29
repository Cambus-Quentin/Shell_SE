#ifndef __SHELL__H__
#define __SHELL__H__

#include <stdio.h>
#define CYAN "\033[0;36m"
#define RESET "\033[0m"

/* Affichage du chevron etdu la localisation en couleur */
void print_prompt(char *pwd_path);

#endif