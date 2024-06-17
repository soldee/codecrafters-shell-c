#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    char *cmd;
    cmd = strtok(input, " ");

    if (strcmp(cmd, "exit") == 0) {
      char *exit_code_str;
      exit_code_str = strtok(NULL, " ");
      int exit_code = atoi(exit_code_str);  // atoi returns 0 if an invalid string is returned
      return exit_code;

    } else if (strcmp(cmd, "echo") == 0) {
        char* echoString = input; 
        echoString += 5; // add len("echo ") to pointer
        printf("%s\n", echoString);

    } else {
        printf("%s: command not found\n", input);
        fflush(stdout);
    }
  }
  
  return 0;
}