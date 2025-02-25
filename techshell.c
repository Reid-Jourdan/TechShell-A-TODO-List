#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>

int TOKEN_SIZE = sizeof(char) * 1024;

struct ShellCommand
{
    char *command;
    char **args;
    // char *input_file;
    // char *output_file;
    // int append;
};

char *read_line();
struct ShellCommand split_line(char *input);
int execute_cmd(struct ShellCommand command);

// char *read_line();
// struct ShellCommand ParseCommandLine(char* input); // Process the user input (As a shell command)
// void ExecuteCommand(struct ShellCommand command); //Execute a shell command

char *read_line()
{
    int bufferSize = 1024;
    int position = 0;
    char *currCommand = malloc(sizeof(char) * bufferSize);
    int character;

    while (1)
    {
        character = getchar();
        if (character == EOF || character == '\n' || character == '\0')
        {
            currCommand[position] = '\0';
            return currCommand;
        }

        currCommand[position++] = character;

        if (position >= bufferSize)
        {                                        // the current string being built has reach the allocated memory size
            bufferSize += (1024 * sizeof(char)); // add another 1024 characters
            currCommand = realloc(currCommand, bufferSize);
        }
    }
}

struct ShellCommand split_line(char *line)
{
    int buffsize = 1024, position = 0;
    char **tokens = malloc(buffsize * sizeof(char *));
    struct ShellCommand commandLine;

    char *token = strtok(line, " ");
    while (token != NULL)
    {
        tokens[position++] = token;

        if (position >= buffsize)
        {
            buffsize += TOKEN_SIZE;
            tokens = realloc(tokens, buffsize * sizeof(char *));
        }

        token = strtok(NULL, " ");
    }
    tokens[position] = NULL;

    commandLine.command = tokens[0];
    commandLine.args = tokens;
    return commandLine;

    // char ** newTokens = malloc(buffsize * sizeof(char * ));
    // int j = 0;

    // for (int i=0; i < position; i++){
    //     if (strcmp(tokens[i], "&&") == 0){
    //         j = 0;
    //         if(execute_cmd(newTokens) == 2){
    //             break;
    //         }; //if fail then break, if succeed the continue
    //         continue;
    //     } else if(strcmp(tokens[i], "||") == 0){
    //         j = 0;
    //         if(execute_cmd(newTokens) != 2){
    //             break;
    //         } //if fail continue if succeed break
    //         continue;
    //     } else if(strcmp(tokens[i], ";") == 0){
    //         j = 0;
    //         execute_cmd(newTokens);
    //         continue;
    //     } else if(strcmp(tokens[i], "|") == 0){

    //     } else if(strcmp(tokens[i], ">") == 0){

    //     } else if(strcmp(tokens[i], "<") == 0){
    //         newTokens[j] = tokens[++i];
    //         execvp(newTokens[0], newTokens);
    //         break;
    //     }
    //     newTokens[j] = tokens[i];
    //     j++;
    // }
    // newTokens[j] = NULL;
    // return newTokens;
}

int execute_cmd(struct ShellCommand commandLine)
{
    if (commandLine.command == NULL || strcmp(commandLine.command, "") == 0)
    {
        return 1;
    }

    if (strcmp(commandLine.command, "exit") == 0)
    {
        return 0;
    }

    if (strcmp(commandLine.command, "cd") == 0)
    {
        if (commandLine.args[1] == NULL)
        {
            chdir(getenv("HOME"));
        }
        else
        {
            if (chdir(commandLine.args[1]) != 0)
            {
                perror("error occurred\n");
            }
        }
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0)
    {
        int i = 0;
        while (commandLine.args[i] != NULL)
        {
            if (strcmp(commandLine.args[i], "<") == 0)
            {
                FILE *infile = fopen(commandLine.args[i + 1], "r");
                dup2(fileno(infile), 0);
                fclose(infile);
                commandLine.args[i] = NULL;
                break;
            }
            else if (strcmp(commandLine.args[i], ">") == 0)
            {
                FILE *outfile = fopen(commandLine.args[i + 1], "w");
                dup2(fileno(outfile), 1);
                fclose(outfile);
                commandLine.args[i] = NULL;
                break;
            }
            else if (strcmp(commandLine.args[i], ">>") == 0)
            {
                FILE *outfile = fopen(commandLine.args[i + 1], "a");
                dup2(fileno(outfile), 1);
                fclose(outfile);
                commandLine.args[i] = NULL;
                break;
            }
            else if (strcmp(commandLine.args[i], "|") == 0)
            {
                // FILE* outfile = fopen(commandLine.args[i+1], "w");
                // dup2(fileno(outfile), 1);
                // fclose(outfile);
                // execvp(commandLine.args[i+1], &commandLine.args[i+1]);// i+1 -> end

                // break;

                // int pipefd[2];
                // pipe(pipefd);
                // pid_t pid2 = fork();
                // if (pid2 == 0){
                //     commandLine.args[i] = NULL;
                //     close(pipefd[0]);
                //     dup2(pipefd[1], 1);
                //     close(pipefd[1]);
                //     execvp(commandLine.command, commandLine.args);//0 -> i-1
                // } else {
                //     int j = i+1;
                //     while(commandLine.args[j] != NULL){
                //         printf("%s", commandLine.args[j]);
                //         j++;
                //     }
                // close(pipefd[1]);
                // dup2(pipefd[0], 0);
                // close(pipefd[0]);
                // execlp(commandLine.args[i+1], commandLine.args[i+1],(char *)NULL);// i+1 -> end

                // break;
                // }
            }
            else if (strcmp(commandLine.args[i], "&&") == 0)
            {
                commandLine.args[i] = NULL;
                if(execvp(commandLine.command, &commandLine.args[0]) == -1){
                    printf("diff error occured");
                }
                commandLine.command = commandLine.args[i+1];
                commandLine.args = &commandLine.args[i+1];
            }

            i++;
        }
        if (execvp(commandLine.command, commandLine.args) == -1)
        {
            perror("error occurred\n");
        }
        // else if (pid < 0)
        // {
        //     perror("error occurred\n");
        // }
        else
        {
            // int status;
            waitpid(pid, NULL, 0);
            /*
            int i = 0;
            while (commandLine.args[i] != NULL)
            {
                if (strcmp(commandLine.args[i], "&&") == 0)
                {
                    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
                    {
                        struct ShellCommand nextCommand = split_line(commandLine.args[i + 1]);
                        return execute_cmd(nextCommand);
                    }
                    else
                    {
                        return 1;
                    }
                }
            }
            i++;
            */
        }
    }
    return 1;
}

void loop()
{
    // ShellCommand line = {
    //     .command = NULL,
    //     .args = NULL,
    //     .input_file = NULL,
    //     .output_file = NULL,
    //     .append = 0
    // }
    char *input;
    struct ShellCommand command;
    int continueLoop = 1;
    char *tscwd;
    do
    {
        tscwd = getcwd(NULL, 0);
        printf("%s$ ", tscwd);
        input = read_line();
        command = split_line(input);
        continueLoop = execute_cmd(command); // will be 0 if exit was the command, this will end the loop
        //   free(command);
    } while (continueLoop);
}

int main()
{
    loop();
    return 0;
}

// int main() // MAIN
// {
// char* input;
// struct ShellCommand command;
// repeatedly prompt the user for input
// for (;;)
// {
// input = CommandPrompt();
// // parse the command line
// command = ParseCommandLine(input);
// // execute the command
// ExecuteCommand(command);
// }
// exit(0);
// }
