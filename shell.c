#include "shell.h"
#include "pipe.h"
#include "readline.h"
#include "intern_cmd.h"
#include "extern_cmd.h"
#include <unistd.h>


void print_prompt(char *pwd_path)
{
    printf(CYAN "%s" RESET, pwd_path);
    printf("> ");
}

void run_shell(int pwd_loc, int path_loc, char **envp)
{
    for (;;)
    {
        print_prompt(envp[pwd_loc] + 4);
        fflush(stdout);
        char *line = readline();
        char **words = split_in_words(line);
        int nb_pipe = 0;
        pcommand *tabCommand = (pcommand *)malloc(sizeof(pcommand));

        nb_pipe = pipe_parsing(words, tabCommand);

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
            if (tabCommand[cmd_indice] != NULL && (exec_intern_cmd(tabCommand[cmd_indice]->cmds, envp) == -1)) /* Utilisation des fonctions externes */
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

                    exec_extern_cmd(tabCommand[cmd_indice]->cmds, envp, path_loc);

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
        for (int i = 0; i <= nb_pipe; i++)
        {
            waitpid(tab_pid[i], &status, 0);
        }

        free(words);
        free(line);
    }
}