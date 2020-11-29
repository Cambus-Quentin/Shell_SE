#ifndef __INTERN_CMD__H__
#define __INTERN_CMD__H__

/* fonnction interne pwd*/
int pwd(char **envp, char **args);

/*fonction de parsing pour cd avec chemin absolu et relatif 
A remplacer par chdir */
char *path_parsing(char *path, char **envp, int pwd_loc, int home_loc);

/* fonction cd */
int cd(char **envp, char **args, int indice_pwd, int indice_home);

/* fonction d'execution d'un commande interne*/
int exec_intern_cmd(char **cmd_name, char **envp);


#endif