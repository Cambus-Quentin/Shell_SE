#include "readline.h"

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
  int home_loc = find_env(envp, "HOME=");

  for (;;)
  {
    printf("> ");
    fflush(stdout);
    char *line = readline();
    char **words = split_in_words(line);
    // for (int i = 0; words[i] != NULL; i++)
    //   printf("%s\n", words[i]);

    char *command = words[0];
    if (command == NULL)
    {
      free(words);
      free(line);
      continue;
    }

    char *slash = (char *)malloc(sizeof(char) * BUFFER_LENGTH);
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
        perror("FORK : NO_CHILD_CREATED");
        exit(-1);
      case 0:
      { /* child code */
        strcat(slash, command);
        char *path = (char *)malloc(strlen(envp[path_loc]));
        strcpy(path, envp[path_loc]);

        char *path_part = strtok(path, ":");
        char *final_command = (char *)malloc(sizeof(char) * BUFFER_LENGTH);
        int error = -1;
        while (path_part != NULL)
        {
          strcat(final_command, path_part);
          strcat(final_command, slash);
          if ((error = open(final_command, O_RDONLY)) != -1)
            break;
          path_part = strtok(NULL, ":");
          free(final_command);
          final_command = (char *)malloc(sizeof(char) * BUFFER_LENGTH);
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
        free(final_command);
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
