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

int pwd(char **envp, char **args)
{
  if (args[0] != NULL && strcmp(args[0], "--help") == 0)
  {
    printf(" pwd - output the current working directory \n      pwd \n ");
    printf(BOLDBLACK "DESCRIPTION \n pwd " RESET);
    printf("outputs (prints) the current working directory. \n");
    printf("Code de retour : \nRenvoie 0 ");
  }
  else
  {
    /* find env à enlever */
    char *printed_path = envp[find_env(envp, "PWD")];
    printed_path = &printed_path[4];
    printf("%s\n", printed_path);
  }
  return 0;
}

// char *absolute_path(char *path, char **envp, int indice_pwd)
// {
//   char *final_path = (char *)malloc(255);
//   char *dir = strtok(path, "/");
//   char *dir_current, *dir_cur_before;
//   if (dir == NULL)
//   {
//     final_path = envp[indice_pwd] + 4;
//     dir = strtok(NULL, "l");
//   }
//   /* walk through other tokens */
//   while (dir != NULL)
//   {
//     if (dir = "..")
//     {
//       dir_current = strtok(final_path,"/");
//       dir_current = strtok(NULL,"/");
//       while (dir_current == NULL ){
//         dir_cur_before = dir_current;
//       }
//     }
//     strcat(final_path, dir);
//     dir = strtok(NULL, "/");
//   }
// }

char *path_parsing(char *path, char **envp, int pwd_loc, int home_loc)
{
  char *dirs[MAX_NWORDS];
  int ndirs = 0;
  char *cur = path;
  char c;
  dirs[0] = NULL;
  int dec = 0;

  if (path[0] == '~')
  {
    dec = home_loc;
    cur++;
  }
  else if (path[0] != '/')
  {
    dec = pwd_loc;
  }

  if (dec != 0)
  {
    char *begin;
    strcpy(begin, envp[dec]);

    char *seg = strtok(begin, "/");
    seg = strtok(NULL, "/");
    while (seg != NULL)
    {
      dirs[ndirs++] = seg;
      seg = strtok(NULL, "/");
    }
  }

  while ((c = *cur) != 0)
  {
    char *dir = NULL;
    char *start;
    switch (c)
    {
    case ' ':
    case '\t':
      cur++;
      break;
    case '/':
      cur++;
      break;
    case '\\':
      /* Ignore any whitespace */
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
      dir = malloc((cur - start + 1) * sizeof(char));
      strncpy(dir, start, cur - start);
      dir[cur - start] = 0;
    }
    if (dir)
    {
      dirs[ndirs++] = dir;
      dirs[ndirs] = NULL;
    }
  }
  size_t size = (ndirs + 1) * sizeof(char *);
  char **tmp = malloc(size);
  memcpy(tmp, dirs, size);
  return tmp;
}

int cd(char **envp, char **args, int indice_pwd, int indice_home)
{
  if (args[0] == NULL)
  {
    char *home = envp[indice_home] + 5;
    envp[indice_pwd] = home;
  }
  else if (strcmp(args[0], "--help") == 0)
  {
    /* TO_DO : Ajoutez manuel du cd*/
    printf("Man cd");
  }
  else
  {

    //error = open(final_command, O_RDONLY | O_DIRECTORY);
  }
  return 0;
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
  char **tmp = malloc(size);
  memcpy(tmp, words, size);
  return tmp;
}

int main(int argc, char **argv, char **envp)
{
  int pwd_loc = find_env(envp, "PWD=");
  int path_loc = find_env(envp, "PATH=");
  int home_loc = find_env(envp, "HOME=");

  for (;;)
  {
    printf("> ");
    fflush(stdout);
    char *line = readline();
    char **words = split_in_words(line);
    for (int i = 0; words[i] != NULL; i++)
      printf("%s\n", words[i]);

    char *command = words[0];
    char *slash = (char *)malloc(1);
    slash[0] = '/';

    if (strcmp(command, "pwd") == 0)
    {
      pwd(envp, words);
    }
    else if (strcmp(command, "cd") == 0)
    {
      cd(envp, words, pwd_loc, home_loc);
    }
    else if (strcmp(command, "exit") == 0)
    {
      exit(0);
    }
    else
    {
      int pid = fork();
      int status;
      switch (pid)
      {
      case -1: /* error */
        perror("FORK : NO_CHIL_CREATED");
        exit(-1);
      case 0:
      { /* child code */
        strcat(slash, command);
        char *path = (char *)malloc(strlen(envp[path_loc]));
        strcpy(path, envp[path_loc]);

        char *path_part = strtok(path, ":");
        char *final_command = (char *)malloc(1);
        int error = -1;
        while (path_part != NULL)
        {
          strcat(final_command, path_part);
          strcat(final_command, slash);
          if (error = open(final_command, O_RDONLY) != -1)
            break;
          path_part = strtok(NULL, ":");
          free(final_command);
          final_command = (char *)malloc(1);
        }
        if (error != -1)
        {
          execve(final_command, words, envp);
        }
        else
        {
          // TODO : CHANGER LES ERREURS
          printf("ERREUR \n");
        }

        break;
      }
      default: /* parent code */
        if (-1 == waitpid(pid, &status, 0))
          perror("waitpid: ");
        break;
      }
    }

    free(words);
    free(line);
  }
  return 0;
}
