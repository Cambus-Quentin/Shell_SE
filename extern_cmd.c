#include "extern_cmd.h"
#include "readline.h"


void exec_extern_cmd(char **cmd, char **envp, int path_loc)
{
    char *slash = (char *)malloc(sizeof(char) * BUFFER_LENGTH);
    slash[0] = '/';

    strcat(slash, cmd[0]);
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
        execve(final_command, cmd, envp);
    }
    else
    {
        printf("La commande executée est erronée. %s\n", final_command);
    }

    free(final_command);
}