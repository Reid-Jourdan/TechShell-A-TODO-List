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
    char seperator;
    // char *input_file;
    // char *output_file;
    // int append;
};

char *read_line();
struct ShellCommand split_line(char *input);
int execute_cmd(struct ShellCommand command);

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
        if (strcmp("&&", token) == 0)
        {
            commandLine.seperator = '&';
        }
        else if (strcmp("||", token) == 0)
        {
            commandLine.seperator = '|';
        }
        else if (strcmp("<", token) == 0)
        {
            commandLine.seperator = '<';
        }
        else if (strcmp(">", token) == 0)
        {
            commandLine.seperator = '>';
        }
        else if (strcmp(">>", token) == 0)
        {
            commandLine.seperator = '.';
        }
        else if (strcmp(";", token) == 0)
        {
            commandLine.seperator = ';';
        }

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
}

int execute_cmd(struct ShellCommand commandLine)
{

    if (commandLine.command == NULL || strcmp(commandLine.command, "") == 0)
    {
        return 1;
    }

    if (strcmp(commandLine.command, "exit") == 0)
    {
        exit(0);
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

    if (commandLine.seperator == '<' || commandLine.seperator == '>'  || commandLine.seperator == '.')
    {
        pid_t child2 = fork();
        int status;
        int flag = 0;
        struct ShellCommand secondCommand;
        secondCommand.args = malloc(1024 * sizeof(char *));
        if (child2 == 0)
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
                }
                 else if (strcmp(commandLine.args[i], ">") == 0)
                {
                    FILE *outfile = fopen(commandLine.args[i + 1], "w");
                    dup2(fileno(outfile), 1);
                    fclose(outfile);
                    commandLine.args[i] = NULL;
                }
                else if (strcmp(commandLine.args[i], ">>") == 0)
                {
                    FILE *outfile = fopen(commandLine.args[i + 1], "a");
                    dup2(fileno(outfile), 1);
                    fclose(outfile);
                    commandLine.args[i] = NULL;
                }
                i++;
            }

            execvp(commandLine.command, commandLine.args);
            printf("There was a problem executing: \"%s\"\n", commandLine.command);
            exit(1);
        }
        else
        {
            waitpid(child2, &status, 0);
        }
    }
    else
    {

        int flag = 0;
        struct ShellCommand secondCommand;
        secondCommand.args = malloc(1024 * sizeof(char *));
        int i = 0;
        while (commandLine.args[i] != NULL)
        {
            // if (strcmp(commandLine.args[i], "<") == 0)
            // {
            //     FILE *infile = fopen(commandLine.args[i + 1], "r");
            //     dup2(fileno(infile), 0);
            //     fclose(infile);
            //     commandLine.args[i] = NULL;
            //     break;
            // }
            // else if (strcmp(commandLine.args[i], ">") == 0)
            // {
            //     FILE *outfile = fopen(commandLine.args[i + 1], "w");
            //     dup2(fileno(outfile), 1);
            //     fclose(outfile);
            //     commandLine.args[i] = NULL;
            //     break;
            // }
            // else if (strcmp(commandLine.args[i], ">>") == 0)
            // {
            //     FILE *outfile = fopen(commandLine.args[i + 1], "a");
            //     dup2(fileno(outfile), 1);
            //     fclose(outfile);
            //     commandLine.args[i] = NULL;
            //     break;
            // }
            if (strcmp(commandLine.args[i], "|") == 0)
            {
            }
            else if (strcmp(commandLine.args[i], "&&") == 0)
            {
                flag = 1738;
                int temp = i;

                secondCommand.command = commandLine.args[++i];
                int j = 0;
                while (commandLine.args[i] != NULL)
                {
                    secondCommand.args[j] = commandLine.args[i];
                    i++;
                    j++;
                }
                commandLine.args[temp] = NULL;
                break;
            }
            else if (strcmp(commandLine.args[i], "||") == 0)
            {
                flag = 420;
                int temp = i;

                secondCommand.command = commandLine.args[++i];
                int j = 0;
                while (commandLine.args[i] != NULL)
                {
                    secondCommand.args[j] = commandLine.args[i];
                    i++;
                    j++;
                }
                commandLine.args[temp] = NULL;
                break;
            }
            else if (strcmp(commandLine.args[i], ";") == 0)
            {
                flag = 69;
                int temp = i;

                secondCommand.command = commandLine.args[++i];
                int j = 0;
                while (commandLine.args[i] != NULL)
                {
                    secondCommand.args[j] = commandLine.args[i];
                    i++;
                    j++;
                }
                commandLine.args[temp] = NULL;
                break;
            }

            i++;
        }
        pid_t child2 = fork();
        int status;
        if (child2 == 0)
        {
            execvp(commandLine.command, commandLine.args);
            printf("There was a problem executing: \"%s\"\n", commandLine.command);
            exit(1);
        }
        else
        {
            waitpid(child2, &status, 0);
        }

        pid_t pid = fork();
        if (pid == 0)
        {
            if (flag == 1738 && WIFEXITED(status) && WEXITSTATUS(status) == 0)
            {

                if (execvp(secondCommand.command, secondCommand.args) == 1)
                {
                    perror("error occurred\n");
                }
                printf("There was a problem executing \"%s\"\n", secondCommand.command);
                exit(1);
            }
            else if (flag == 420 && WIFEXITED(status) && WEXITSTATUS(status) == 1)
            {
                if (execvp(secondCommand.command, secondCommand.args) == 1)
                {
                    perror("error occurred\n");
                }
                printf("There was a problem executing \"%s\"\n", secondCommand.command);
                exit(1);
            }
            else if (flag == 69)
            {
                if (execvp(secondCommand.command, secondCommand.args) == 1)
                {
                    perror("error occurred\n");
                }
                printf("There was a problem executing \"%s\"\n", secondCommand.command);
                exit(1);
            }
            exit(0);
        }

        else
        {
            int statusMain;
            waitpid(pid, &statusMain, 0);
        }

        return 1;
    }
}

void loop()
{

    char *input;
    struct ShellCommand command;
    int continueLoop = 1;
    char *tscwd;
    do
    {
        tscwd = getcwd(NULL, 0);             // get the current working directory
        printf("%s$ ", tscwd);               // print the current working directory
        input = read_line();                 // read the input from the user
        command = split_line(input);         // split the input into a command and arguments
        continueLoop = execute_cmd(command); // will be 0 if exit was the command, this will end the loop
        free(command.args);                  // free the memory allocated for the arguments
        free(command.command);               // free the memory allocated for the command
        free(tscwd);                         // free the memory allocated for the current working directory
    } while (continueLoop);
}

int main()
{
    loop();
    return 0;
}
