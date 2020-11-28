#include "pipe.h"
#include <stdlib.h>


pcommand *find_pipe(char ***words)
{
    pcommand *cmds = (pcommand *)malloc(sizeof(pcommand));
    int i = 0;
    int debut = 0;
    int nb_pipes = 0;
    for (; *words[i] != NULL; i++)
    {
        if (cmds[i] == NULL)
        {
            cmds = realloc(cmds, (i + 1) * sizeof(pcommand) * 2);
            cmds[i] = (pcommand)malloc(sizeof(command));
        }
        if (*words[i][0] == '|')
        {

            cmds[i]->args = words[debut];
            // cmds[j]->sortie = fp[1];
            // cmds[j]->entree = -1;
            cmds[i++]->args[i - debut] = NULL;
            debut = i + 1;
        }
    }
    cmds[--i]->args = words[debut];
    cmds[i]->args[i - debut] = NULL;
    // cmds[j]->entree = fp[0];
    // cmds[j]->sortie = -1;
    return cmds;
}

void exec_pipe (char** command, char** dos){
    
}
