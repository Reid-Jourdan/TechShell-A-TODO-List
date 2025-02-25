#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#define MAX_ARGS 64
#define MAX_COMMANDS 10

typedef struct {
    char *args[MAX_ARGS];
    char *input_file;
    char *output_file;
    int append;
    int background;
    int pipe_out;
} Command;

typedef struct {
    Command commands[MAX_COMMANDS];
    int cmd_count;
    int and_operator;    // 1 for &&, 0 for default, -1 for ||
} CommandSequence;

// Function prototypes
char *read_line();
CommandSequence parse_line(char *line);
void execute_sequence(CommandSequence *seq);
int execute_command(Command *cmd, int in_fd, int out_fd);
void handle_io_redirection(Command *cmd);

char *read_line() {
    int bufsize = 1024;
    int position = 0;
    char *buffer = malloc(bufsize);
    int c;

    while (1) {
        c = getchar();
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        }
        buffer[position++] = c;
        
        if (position >= bufsize) {
            bufsize += 1024;
            buffer = realloc(buffer, bufsize);
        }
    }
}

CommandSequence parse_line(char *line) {
    CommandSequence seq = {0};
    char *token;
    char *saveptr;
    int arg_index = 0;
    int cmd_index = 0;

    token = strtok_r(line, " \t\n", &saveptr);
    while (token != NULL) {
        if (strcmp(token, "|") == 0) {
            seq.commands[cmd_index].args[arg_index] = NULL;
            seq.commands[cmd_index].pipe_out = 1;
            cmd_index++;
            arg_index = 0;
        }
        else if (strcmp(token, ">") == 0) {
            seq.commands[cmd_index].output_file = strtok_r(NULL, " \t\n", &saveptr);
            seq.commands[cmd_index].append = 0;
        }
        else if (strcmp(token, ">>") == 0) {
            seq.commands[cmd_index].output_file = strtok_r(NULL, " \t\n", &saveptr);
            seq.commands[cmd_index].append = 1;
        }
        else if (strcmp(token, "<") == 0) {
            seq.commands[cmd_index].input_file = strtok_r(NULL, " \t\n", &saveptr);
        }
        else if (strcmp(token, "&&") == 0) {
            seq.and_operator = 1;
            break;
        }
        else if (strcmp(token, "||") == 0) {
            seq.and_operator = -1;
            break;
        }
        else if (strcmp(token, "&") == 0) {
            seq.commands[cmd_index].background = 1;
        }
        else if (strcmp(token, ";") == 0) {
            seq.commands[cmd_index].args[arg_index] = NULL;
            cmd_index++;
            arg_index = 0;
        }
        else {
            seq.commands[cmd_index].args[arg_index++] = token;
            if (arg_index >= MAX_ARGS-1) break;
        }
        token = strtok_r(NULL, " \t\n", &saveptr);
    }
    
    seq.commands[cmd_index].args[arg_index] = NULL;
    seq.cmd_count = cmd_index + 1;
    return seq;
}

void handle_io_redirection(Command *cmd) {
    if (cmd->input_file) {
        int fd = open(cmd->input_file, O_RDONLY);
        if (fd < 0) {
            perror("open input");
            exit(EXIT_FAILURE);
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }
    
    if (cmd->output_file) {
        int flags = O_WRONLY | O_CREAT;
        flags |= cmd->append ? O_APPEND : O_TRUNC;
        int fd = open(cmd->output_file, flags, 0644);
        if (fd < 0) {
            perror("open output");
            exit(EXIT_FAILURE);
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
}

int execute_command(Command *cmd, int in_fd, int out_fd) {
    if (cmd->args[0] == NULL) return 0;

    // Handle builtins
    if (strcmp(cmd->args[0], "cd") == 0) {
        if (cmd->args[1] && chdir(cmd->args[1]) != 0) {
            perror("cd");
        }
        return 0;
    }
    if (strcmp(cmd->args[0], "exit") == 0) exit(0);

    pid_t pid = fork();
    if (pid == 0) { // Child
        if (in_fd != STDIN_FILENO) {
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }
        if (out_fd != STDOUT_FILENO) {
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }
        
        handle_io_redirection(cmd);
        execvp(cmd->args[0], cmd->args);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else if (pid < 0) {
        perror("fork");
        return -1;
    }
    
    if (!cmd->background) {
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
    return 0;
}

void execute_pipeline(Command *cmds, int count) {
    int prev_pipe[2];
    int next_pipe[2];
    int in_fd = STDIN_FILENO;

    for (int i = 0; i < count; i++) {
        if (i < count - 1) {
            pipe(next_pipe);
        }

        int out_fd = (i < count - 1) ? next_pipe[1] : STDOUT_FILENO;
        int status = execute_command(&cmds[i], in_fd, out_fd);

        if (in_fd != STDIN_FILENO) close(in_fd);
        if (out_fd != STDOUT_FILENO) close(out_fd);
        
        in_fd = next_pipe[0];
        
        if (status != 0 && i < count - 1) break;
    }
}

void execute_sequence(CommandSequence *seq) {
    int last_status = 0;
    
    for (int i = 0; i < seq->cmd_count; i++) {
        if (seq->and_operator) {
            if ((seq->and_operator == 1 && last_status != 0) ||
                (seq->and_operator == -1 && last_status == 0)) break;
        }
        
        if (seq->commands[i].pipe_out) {
            int cmd_count = 1;
            while (i + cmd_count < seq->cmd_count && seq->commands[i + cmd_count].pipe_out) {
                cmd_count++;
            }
            execute_pipeline(&seq->commands[i], cmd_count);
            i += cmd_count - 1;
        } else {
            last_status = execute_command(&seq->commands[i], STDIN_FILENO, STDOUT_FILENO);
        }
    }
}

void sigchld_handler(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main() {
    signal(SIGCHLD, sigchld_handler);
    
    while (1) {
        char *cwd = getcwd(NULL, 0);
        printf("%s$ ", cwd);
        free(cwd);
        
        char *line = read_line();
        if (!line) continue;
        
        CommandSequence seq = parse_line(line);
        execute_sequence(&seq);
        
        free(line);
    }
    return 0;
}