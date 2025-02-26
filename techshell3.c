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
struct ShellCommand *split_line(char *line, int *cmd_count);
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

struct ShellCommand *split_line(char *line, int *cmd_count)
{
    int buffsize = 1024, position = 0;
    char **tokens = malloc(buffsize * sizeof(char *));
    struct ShellCommand *commands = malloc(buffsize * sizeof(struct ShellCommand));
    int commandIndex = 0;

    char *token = strtok(line, " ");
    while (token != NULL)
    {
        if (strcmp(token, "&&") == 0)
        {
            tokens[position] = NULL; // End previous command argument list
            commands[commandIndex].command = tokens[0];
            commands[commandIndex].args = tokens;
            commandIndex++;

            // Allocate new memory for the next command
            tokens = malloc(buffsize * sizeof(char *));
            position = 0;
        }
        else
        {
            tokens[position++] = token;
        }

        if (position >= buffsize)
        {
            buffsize += TOKEN_SIZE;
            tokens = realloc(tokens, buffsize * sizeof(char *));
        }

        token = strtok(NULL, " ");
    }

    tokens[position] = NULL;  // Null-terminate the last command
    commands[commandIndex].command = tokens[0];
    commands[commandIndex].args = tokens;
    commandIndex++;

    *cmd_count = commandIndex;
    return commands;  // Return the pointer to the array of commands
}



int execute_cmds(struct ShellCommand *commands, int cmd_count)
{
    for (int i = 0; i < cmd_count; i++)
    {
        struct ShellCommand command = commands[i];

        if (command.command == NULL || strcmp(command.command, "") == 0)
        {
            continue;
        }

        if (strcmp(command.command, "exit") == 0)
        {
            return 0;
        }
        
        if (commandLine.command == NULL) {
            fprintf(stderr, "Error: No command provided\n");
            return 1;
        }

        if (strcmp(command.command, "cd") == 0)
        {
            if (command.args[1] == NULL)
            {
                chdir(getenv("HOME"));
            }
            else if (chdir(command.args[1]) != 0)
            {
                perror("cd failed");
            }
            continue;
        }

        pid_t pid = fork();
        int status;
        if (pid == 0)
        {
            execvp(command.command, command.args);
            perror("Command execution failed");
            exit(EXIT_FAILURE);
        }
        else if (pid > 0)
        {
            waitpid(pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            {
                break; // Stop execution if a command fails
            }
        }
        else
        {
            perror("Fork failed");
            return 1;
        }
    }

    return 1;
}


void loop()
{
    char *input;
    int cmd_count;
    struct ShellCommand *commands;

    do
    {
        char *cwd = getcwd(NULL, 0);
        printf("%s$ ", cwd);
        free(cwd);

        input = read_line();
        commands = split_line(input, &cmd_count);  // Correct function call
        free(input);

        if (cmd_count > 0)
        {
            execute_cmds(commands, cmd_count);  // Execute all parsed commands
        }

        // Free allocated memory for each command
        for (int i = 0; i < cmd_count; i++)
        {
            free(commands[i].args);
        }
        free(commands);

    } while (1);
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
