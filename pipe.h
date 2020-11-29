#ifndef __PIPE__H__
#define __PIPE__H__
#include <stdlib.h>
#include <string.h>

typedef struct c
{
  char **cmds;
} command, *pcommand;

pcommand *find_pipe(char*** words);

int pipe_parsing(char **words, pcommand *tabCommand);

#endif