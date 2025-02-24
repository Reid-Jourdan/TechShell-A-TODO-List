#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>

char *read_line();
char **split_line(char *);
int execute_cmd(char **);

int TOKEN_SIZE = sizeof(char) * 1024;

char *read_line() {
  int bufferSize = 1024;
  int position = 0;
  char *currCommand = malloc(sizeof(char) * bufferSize);
  int character;

  while (1) {

        character = getchar();
        if (character == EOF || character == '\n' || character == '\0') {
          currCommand[position] = '\0';
          return currCommand;
        } 
          
        currCommand[position++] = character;
        
    
        if (position >= bufferSize) { // the current string being built has reach the allocated memory size
          bufferSize += (1024 * sizeof(char)); // add another 1024 characters
          currCommand = realloc(currCommand, bufferSize);
    
      }
  }
}

char * * split_line(char * line) {
    int buffsize = 1024, position = 0;
    char ** tokens = malloc(buffsize * sizeof(char * ));
    char * token;

    token = strtok(line, " ");
        while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= buffsize) {
            buffsize += TOKEN_SIZE;
            tokens = realloc(tokens, buffsize * sizeof(char*));
        }

        token = strtok(NULL, " ");
    }

    tokens[position] = NULL;
    return tokens;
}

int execute_cmd(char **args) {
  if (args[0] == NULL || strcmp(args[0], "") == 0) {
      return 1;
  }
  
  if (strcmp(args[0], "exit") == 0) {
      return 0;
  }
  
  if (strcmp(args[0], "cd") == 0) {
      if (chdir(args[1]) != 0) {
          perror("cd");
      }
      return 1;
  }
  
  pid_t pid = fork();
  if (pid == 0) {
      if (execvp(args[0], args) == -1) {
          perror("error occurred\n");
      }
      exit(EXIT_FAILURE);
  } else if (pid < 0) {
      perror("fork");
  } else {
      waitpid(pid, NULL, 0);
  }
  return 1;
}

void loop() {
    char *line;
    char **args;
    int continueLoop = 1;

    do {
      printf("> ");
      line = read_line();
      args = split_line(line);
      continueLoop = execute_cmd(args); // will be 0 if exit was the command, this will end the loop
      free(line);
      free(args);
    } while (continueLoop);
}

int main() {
    loop();
    return 0;
}