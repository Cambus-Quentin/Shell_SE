int pwd(char **envp, char **args);

char *path_parsing(char *path, char **envp, int pwd_loc, int home_loc);

int cd(char **envp, char **args, int indice_pwd, int indice_home);