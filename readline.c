#include "readline.h"
#include "intern_cmd.h"
#include "pipe.h"
#include "shell.h"
#include "extern_cmd.h"

//STRNCMP

int find_env(char **envp, char *pattern)
{
  int len;
  for (int i = 0; envp[i] != NULL; i++)
  {
    len = strlen(pattern);

    if (strncmp(envp[i], pattern, len) == 0)
    {
      return i;
    }
  }
  return -1;
}

/*
 * Read a line from standard input into a newly allocated 
 * array of char. The allocation is via malloc(size_t), the array 
 * must be freed via free(void*).
 */

char *readline(void)
{
  static char buffer[BUFFER_LENGTH];
  int offset = 0;
  for (;;)
  {
    char c = fgetc(stdin);
    if (c == EOF)
    {
      printf("PANIC: EOF on stdin\n");
      exit(-1);
    }
    if (c == '\n')
      break;
    buffer[offset++] = c;
  }
  buffer[offset++] = '\0';
  char *line = malloc(offset);
  strcpy(line, buffer);
  return line;
}

/* 
 * Split the string in words, according to the simple shell grammar.
 * Returns a null-terminated array of words.
 * The array has been allocated by malloc, it must be freed by free.
 */
char **split_in_words(char *line)
{
  static char *words[MAX_NWORDS];
  int nwords = 0;
  char *cur = line;
  char c;
  words[0] = NULL;
  while ((c = *cur) != 0)
  {
    char *word = NULL;
    char *start;
    switch (c)
    {
    case ' ':
    case '\t':
      /* Ignore any whitespace */
      cur++;
      break;
    case '<':
      word = "<";
      cur++;
      break;
    case '>':
      word = ">";
      cur++;
      break;
    case '\\':
      word = ">";
      cur++;
      break;
    case '|':
      word = "|";
      cur++;
      break;
    case ';':
      word = ";";
      cur++;
      break;
    case '&':
      word = "&";
      cur++;
      break;
    default:
      /* Another word */
      start = cur;
      if (c == '"')
      {
        c = *++cur;
        while (c != '"')
          c = *++cur;
        cur++;
      }

      else
      {
        while (c)
        {
          c = *++cur;
          switch (c)
          {
          case 0:
          case ' ':
          case '\t':
          case '<':
          case '>':
          case '|':
          case ';':
          case '&':
            c = 0;
            break;
          default:;
          }
        }
      }
      word = malloc((cur - start + 1) * sizeof(char));
      strncpy(word, start, cur - start);
      word[cur - start] = 0;
    }
    if (word)
    {
      words[nwords++] = word;
      words[nwords] = NULL;
    }
  }
  size_t size = (nwords + 1) * sizeof(char *);
  char **tmp = (char **)malloc(size);
  memcpy(tmp, words, size);
  return tmp;
}

int main(int argc, char **argv, char **envp)
{
  int pwd_loc = find_env(envp, "PWD=");
  int path_loc = find_env(envp, "PATH=");
  for (;;)
  {
    print_prompt(envp[pwd_loc] + 4);
    fflush(stdout);
    char *line = readline();
    char **words = split_in_words(line);

    int debut = 0;
    int nb_pipe = 0;
    int i = 0;
    pcommand *tabCommand = (pcommand *)malloc(sizeof(pcommand));
    for (; words[i] != NULL; i++)
    {
      if (strcmp(words[i], "|") == 0)
      {
        if (tabCommand[nb_pipe] == NULL)
        {
          tabCommand = realloc(tabCommand, (nb_pipe + 1) * sizeof(pcommand) * 2);
          tabCommand[nb_pipe] = (pcommand)malloc(sizeof(command));
        }
        tabCommand[nb_pipe]->args = &words[debut];
        tabCommand[nb_pipe++]->args[i - debut] = NULL;
        debut = i + 1;
      }
    }
    tabCommand[nb_pipe] = (pcommand)malloc(sizeof(command));
    tabCommand[nb_pipe]->args = &words[debut];
    tabCommand[nb_pipe]->args[i - debut] = NULL;

    int fps[nb_pipe * 2];

    for (int v = 0; v < nb_pipe; v++)
    {
      if (pipe(fps + v * 2) < 0)
      {
        printf("Error pipe\n");
        exit(1);
      }
    }

    int status;
    int tab_pid[nb_pipe];

    for (int cmd_indice = 0; cmd_indice <= nb_pipe; cmd_indice++)
    {
      /* Déterminer si l'entrée correspond à une commande interne */
      if (tabCommand[cmd_indice] != NULL && (exec_intern_cmd(tabCommand[cmd_indice]->args, envp) == -1)) /* Utilisation des fonctions externes */
      {

        int pid = fork();
        int status;
        switch (pid)
        {
        case -1: /* error */
          perror("FORK : NO_CHILD_CREATED");
          exit(-1);
        case 0:
        {

          if (cmd_indice != 0)
          {
            if (dup2(fps[(cmd_indice - 1) * 2], STDIN_FILENO) == -1)
            {
              printf("Error 2nd termes ou + dup2\n");
              perror("dup2");
              exit(1);
            }
          }

          if (tabCommand[cmd_indice + 1] != NULL)
          {
            if (dup2(fps[(cmd_indice)*2 + 1], STDOUT_FILENO) == -1)
            {
              printf("Pas BON du tout\n ");
              perror("dup2");
              exit(1);
            }
          }

          if (nb_pipe >= 1)
          {
            for (int k = 0; k < nb_pipe * 2; k++)
            {
              if (close(fps[k]) == -1)
              {

                printf("k:%d, fps[k]:%d\n", k, fps[k]);
                perror("close");
                exit(1);
              }
            }
          }

          exec_extern_cmd(tabCommand[cmd_indice]->args, envp, path_loc);

          break;
        }
        default:
          tab_pid[cmd_indice] = pid;
          break;
        }
      }
    }
    
    for (int i = 0; i < nb_pipe; i++)
    {
      close(fps[i * 2]);
      close(fps[i * 2 + 1]);
    }
    for (i = 0; i <= nb_pipe; i++)
    {
      waitpid(tab_pid[i], &status, 0);
    }

    free(words);
    free(line);
  }
  return 0;
}