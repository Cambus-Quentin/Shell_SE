#ifndef __INTERN_CMD__H__
#define __INTERN_CMD__H__

int pwd(char **envp, char **args);

char *path_parsing(char *path, char **envp, int pwd_loc, int home_loc);

int cd(char **envp, char **args, int indice_pwd, int indice_home);

int exec_intern_cmd(char **cmd_name, char **envp);


#endif