#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

#include <string>
#include <sstream>
#include <cstring>

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        perror("Too few arguments\n");
        exit(-1);
    }
    if (argc > 6)
    {
        perror("Too many arguments\n");
        exit(-1);
    }

    char* cmds[5][3];
    int fd[4][2];
    int childPid;

    for (int i = 0; i < argc - 2; i++)
    {
        pipe(fd[i]);
    }

    // Get all commands

    for (int i = 1; i < argc; i++)
    {
        std::string cmd, arg, temp;
        std::stringstream ss(argv[i]);
        
        ss >> cmd;
        while (ss >> temp)
        {
            arg += temp + " ";
        }
        if (!arg.empty()) arg.pop_back();

        cmds[i - 1][0] = strdup(const_cast<char*>(cmd.c_str()));
        cmds[i - 1][1] = strdup(const_cast<char*>(arg.c_str()));
        cmds[i - 1][2] = nullptr;
    }

    for (int i = 0; i < argc - 1; i++)
    {
        if ((childPid = fork()) == -1)
        {
            perror("Forking error");
            exit(-1);
        }

        if (childPid == 0)
        {
            char* command[3] = {strdup(cmds[i][0]), strdup(cmds[i][1]), NULL};
            printf("%s %s\n", command[0], command[1]);
            if (i != 0) // If not the first one
            {
                dup2(fd[i - 1][0], STDIN_FILENO);
            }
            if (i != argc - 2) // If not the last one
            {
                dup2(fd[i][1], STDOUT_FILENO);
            }
            execvp(command[0], command);
        }
    }
}