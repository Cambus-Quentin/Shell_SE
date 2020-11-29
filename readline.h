#ifndef __READLINE__H__
#define __READLINE__H__


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BOLDBLACK "\033[1m\033[30m"
#define RESET "\033[0m"
#define BUFFER_LENGTH 256
#define MAX_NWORDS 256

typedef struct c
{
  char **cmds;
} command, *pcommand;

/* fonction retrounant l'index dans le tableau d'environnement du pattern demandé */
int find_env(char **envp, char *pattern);

char *readline(void);

char **split_in_words(char *line);

int main(int argc, char **argv, char **envp);

#endif