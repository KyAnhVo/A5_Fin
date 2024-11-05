#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int pipes[2][2];
    pipe(pipes[0]);
    pipe(pipes[1]);

    char* cmds[][3] = {
        {(char *)"ls",      (char *)"-ltr",     NULL},
        {(char *)"grep",    (char *)"3377",     NULL},
        {(char *)"wc",      (char *)"-l",       NULL}
    };

    pid_t childPid;

    if ((childPid = fork()) == -1)
    {
        perror("Forking error");
        exit(-1);
    }
    
    if (childPid == 0) // ls -ltr
    {
        dup2(pipes[0][1], STDOUT_FILENO);

        // Close unused pipe ends
        close(pipes[0][0]);
        close(pipes[0][1]);
        close(pipes[1][0]);
        close(pipes[1][1]);

        execvp(cmds[0][0], cmds[0]);
        exit(0);
    }

    if ((childPid = fork()) == -1)
    {
        perror("Forking error");
        exit(-1);
    }
    
    if (childPid == 0) // grep 3377
    {
        dup2(pipes[0][0], STDIN_FILENO);
        dup2(pipes[1][1], STDOUT_FILENO);

        // Close unused pipe ends
        close(pipes[0][0]);
        close(pipes[0][1]);
        close(pipes[1][0]);
        close(pipes[1][1]);

        execvp(cmds[1][0], cmds[1]);
        exit(0);
    }

    // Parent
    dup2(pipes[1][0], STDIN_FILENO);

    // Close unused pipe ends
    close(pipes[0][0]);
    close(pipes[0][1]);
    close(pipes[1][0]);
    close(pipes[1][1]);
    
    execvp(cmds[2][0], cmds[2]);
    return 0;
}