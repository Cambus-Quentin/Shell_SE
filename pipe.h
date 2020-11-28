#ifndef __PIPE__H__
#define __PIPE__H__
#include <stdlib.h>

typedef struct c
{
  int entree;
  int sortie;
  char **args;

} command, *pcommand;

pcommand *find_pipe(char*** words);

#endif