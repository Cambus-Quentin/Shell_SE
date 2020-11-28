#include "intern_cmd.h"
#include "readline.h"

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
        //dirs[ndirs] = NULL;
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
    printf("Man cd\n");
  }
  else
  {
    char *path = path_parsing(args[1], envp, indice_pwd, indice_home);
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

int exec_intern_cmd(char **cmd_name, char **envp)
{
  if (strcmp(cmd_name[0], "pwd") == 0)
  {
    pwd(envp, cmd_name);
  }
  else if (strcmp(cmd_name[0], "cd") == 0)
  {
    cd(envp, cmd_name, find_env(envp, "PWD="), find_env(envp, "HOME="));
  }
  /* ici exit */
  else if (strcmp(cmd_name[0], "exit") == 0)
  {
    exit(0);
  }
  else{
    return -1;
  }
  return 0;
}