#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char CMD_EXIT[] = "exit";
static const char CMD_ECHO[] = "echo";
static const char CMD_TYPE[] = "type";

static const char MSG_BUILTIN[] = "is a shell builtin";


void remove_trailing_newline(char *str) {
  int i = 0;
  while (str[i] != '\n' && str[i] != '\0') {
    i++;
  }
  if (str[i] == '\n') {
    str[i] = '\0';
  }
}

void read_str_until_space(char *input, char *read) {
  int i = 0;
  while (input[i] != ' ' && input[i] != '\0') {
    read[i] = input[i];
    i++;
  }
}

int main() {
  while (1) {
    printf("$ ");
    fflush(stdout);

    char input[100];
    fgets(input, 100, stdin);

    remove_trailing_newline(input);

    if (input[0] == '\0') {
      continue;
    }

    char *cmd;
    cmd = strtok(input, " ");

    if (strcmp(cmd, CMD_EXIT) == 0) {
      char *exit_code_str;
      exit_code_str = strtok(NULL, " ");
      int exit_code = atoi(exit_code_str);  // atoi returns 0 if an invalid string is returned
      return exit_code;

    } else if (strcmp(cmd, CMD_ECHO) == 0) {
        char* echoString = input; 
        echoString += 5; // add len("echo ") to pointer
        printf("%s\n", echoString);

    } else if (strcmp(cmd, CMD_TYPE) == 0) {
      char *typeCommand;
      typeCommand = strtok(NULL, " ");
      if (strcmp(typeCommand, CMD_EXIT) == 0) {
        printf("%s %s\n", CMD_EXIT, MSG_BUILTIN);
      } else if (strcmp(typeCommand, CMD_ECHO) == 0) {
        printf("%s %s\n", CMD_ECHO, MSG_BUILTIN);
      } else if (strcmp(typeCommand, CMD_TYPE) == 0) {
        printf("%s %s\n", CMD_TYPE, MSG_BUILTIN);
      } else {
        printf("%s: not found\n", typeCommand);
      }

    } else {
        printf("%s: command not found\n", input);
        fflush(stdout);
    }
  }
  
  return 0;
}