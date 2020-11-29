#include "pipe.h"


int pipe_parsing(char **words, pcommand *tabCommand) {
  int nb_pipe = 0, debut = 0;
  int i = 0;
  for (; words[i] != NULL; i++)
    {
      if (strcmp(words[i], "|") == 0)
      {
        if (tabCommand[nb_pipe] == NULL)
        {
          tabCommand = realloc(tabCommand, (nb_pipe + 1) * sizeof(pcommand) * 2);
          tabCommand[nb_pipe] = (pcommand)malloc(sizeof(command));
        }
        tabCommand[nb_pipe]->cmds = &words[debut];
        tabCommand[nb_pipe++]->cmds[i - debut] = NULL;
        debut = i + 1;
      }
    }
    tabCommand[nb_pipe] = (pcommand)malloc(sizeof(command));
    tabCommand[nb_pipe]->cmds = &words[debut];
    tabCommand[nb_pipe]->cmds[i - debut] = NULL;

    return nb_pipe;
}
