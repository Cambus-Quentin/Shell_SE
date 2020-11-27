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
#define BUFFER_LENGTH 2048
#define MAX_NWORDS 256

typedef struct c
{
  int entree;
  int sortie;
  char **args;
} command, *pcommand;

/* Permet de trouver l'indice dans le tableau de variable d'environnement de la variable commancant par le pattern souhaiter */
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

/* Commande interne permettant d'afficher le répertoire courant */
int pwd(char **envp, char **args)
{
  if (args[1] != NULL && (strcmp(args[1], "--help") == 0))
  {
    printf(" pwd - output the current working directory \n      pwd \n ");
    printf(BOLDBLACK "DESCRIPTION \n pwd " RESET);
    printf("outputs (prints) the current working directory. \n");
    printf("Code de retour : \nRenvoie 0 ");
  }
  else
  {
    char *test;
    char *printed_path = envp[find_env(envp, "PWD")];
    printed_path = printed_path + 4;
    getcwd(test, BUFFER_LENGTH);
    if (strcmp(printed_path, test) == 0)
      printf("%s\n", printed_path);
    else
      printf("Erreur de synchronisation entre variable d'environnement et current working directory\n");
  }
  return 0;
}

/* Fonction de parsing des chemins absolue et relatif pour cd */
char *path_parsing(char *path, char **envp, int pwd_loc, int home_loc)
{
  char *dirs[MAX_NWORDS];
  int ndirs = 0;
  char *cur = path;
  char c;
  dirs[0] = NULL;
  int dec = -1;

  if (path[0] == '~')
  {
    dec = home_loc;
    cur++;
  }
  else if (path[0] != '/')
  {
    dec = pwd_loc;
  }

  if (dec != -1)
  {
    char *begin = (char *)malloc(MAX_NWORDS);
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
    case '.':
      c = *++cur;
      if (c == '.')
      {
        dirs[ndirs] = NULL;
        ndirs--;
        if (ndirs < 0)
        {
          printf("Erreur mauvais chemin");
          return NULL;
          ;
        }
      }
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
      else if (c == '\'')
      {
        c = *++cur;
        while (c != '\'')
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
          case '/':
            c = 0;
            break;
          case ' ':
          case '\t':
          case '<':
          case '>':
          case '|':
          case ';':
          case '&':
            printf("Chemin incorrect\n");
            return NULL;
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
  char *tmp = (char *)malloc(size);

  for (int i = 0; i < ndirs; i++)
  {
    strcat(tmp, "/");
    strcat(tmp, dirs[i]);
  }
  printf("%s", tmp);
  return tmp;
}

/* Commande interne permettant de se déplacer dans le système de fichiers tout en prenant en compte les erreurs */
int cd(char **envp, char **args, int indice_pwd, int indice_home)
{
  char *res = (char *)malloc(sizeof(char) * BUFFER_LENGTH);
  char *pwd = "PWD=";
  strcat(res, pwd);
  if (args[1] == NULL)
  {
    char *home = envp[indice_home] + 5;
    strcat(res, home);
    envp[indice_pwd] = res;
    chdir(home);
  }
  else if (strcmp(args[1], "--help") == 0)
  {
    /* TO_DO : Ajoutez manuel du cd*/
    printf("man cd\n");
  }
  else
  {
    char *path = path_parsing(args[1], envp, indice_pwd, indice_home);
    printf("PWD : %s\n", envp[indice_pwd]);
    int error = open(path, O_RDONLY | O_DIRECTORY);

    if (error != -1)
    {
      chdir(path);
      envp[indice_pwd] = strcat(res, path);
    }
    else
    {
      printf("Path : %s incorrecte\n", path);
      return -1;
    }
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
    int debut = 0;
    int j = 0;
    int i = 0;
    pcommand *tabCommand = (pcommand *)malloc(sizeof(pcommand));

    for (; words[i] != NULL; i++)
    {
      if (strcmp(words[i], "|") == 0)
      {
        if (tabCommand[j] == NULL)
        {
          tabCommand = realloc(tabCommand, (j + 1) * sizeof(pcommand) * 2);
          tabCommand[j] = (pcommand)malloc(sizeof(command));
        }
        tabCommand[j]->args = &words[debut];
        tabCommand[j++]->args[i - debut] = NULL;
        debut = i + 1;
      }
    }
    tabCommand[j] = (pcommand)malloc(sizeof(command));
    tabCommand[j]->args = &words[debut];
    tabCommand[j]->args[i - debut] = NULL;

    int fp[2];
    for (int z = 0; tabCommand[z] != NULL; z++)
    {
      char *command = tabCommand[z]->args[0];

      char *slash = (char *)malloc(sizeof(char) * BUFFER_LENGTH);
      slash[0] = '/';

      /* Déterminer si l'entrée correspond à une commande interne */
      /* ici pwd */
      if (strcmp(command, "pwd") == 0)
      {
        pwd(envp, tabCommand[z]->args);
      }
      /* ici cd */
      else if (strcmp(command, "cd") == 0)
      {
        cd(envp, tabCommand[z]->args, pwd_loc, home_loc);
      }
      /* ici exit */
      else if (strcmp(command, "exit") == 0)
      {
        exit(0);
      }
      else /* Utilisation des fonctions externes */
      {
        if (j >= 1)
          pipe(fp);
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
          char *path = (char *)malloc(strlen(envp[path_loc]) - 5);
          strcpy(path, envp[path_loc] + 5);

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
            final_command = (char *)malloc(sizeof(char) * BUFFER_LENGTH);
          }
          if (error != -1)
          {
            execve(final_command, tabCommand[z]->args, envp);
          }
          else
          {
            printf("La commande executée est erronée. %s\n", final_command);
          }

          free(final_command);
          break;
        }
        default: /* parent code */
          if (j >= 1)
          {
            close(fp[0]);
            close(fp[1]);
          }
          if (-1 == waitpid(pid, &status, 0))
            perror("waitpid: ");
          break;
        }
      }
    }
    free(words);
    free(line);
  }
  return 0;
}
