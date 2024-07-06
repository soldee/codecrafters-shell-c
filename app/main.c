#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> 
#include <unistd.h>
#include <sys/wait.h>


static const char MSG_BUILTIN[] = "is a shell builtin";

static const char PATH_SEP = ':';
static const char PATH_SLASH = '/';

static const int MAX_INPUT_LEN = 100;
static const int MAX_PATH_LEN = 260;
static const int MAX_ARGV_LEN = 10;


void remove_trailing_newline(char *str) {
  int i = 0;
  while (str[i] != '\n' && str[i] != '\0') {
    i++;
  }
  if (str[i] == '\n') {
    str[i] = '\0';
  }
}

void get_file_from_path_env(char* file_name, char* abs_file_path_buffer, size_t buffer_size) {
  const char* path_original = getenv("PATH");

  // get number of paths in path env variable and path env variable length
  int n_paths = 0;
  size_t i = 0;
  if (path_original[0] != '\0') {
    n_paths++;
  }
  while (path_original[i]) {
    if (path_original[i] == PATH_SEP) {
      n_paths++;
    }
    i++;
  }

  char patharr[i];
  char *path = patharr;
  strcpy(path, path_original);

  // create array of size n_paths and populate array with path string pointers
  char* pathsarr[n_paths];
  pathsarr[0] = &path[0];
  int path_counter = 1;
  i = 0;
  while (path_original[i]) {
    if (path[i] == PATH_SEP) {
      path[i] = '\0';
      if (path_counter < n_paths) {
        pathsarr[path_counter] = &path[i+1];
        path_counter++;
      }
    }
    i++;
  }
  
  // iterate array and find the file in each of the paths
  for (size_t i=0; i < path_counter; i++) {
    if (pathsarr[i][0] != '\0') {
      char last_char = pathsarr[i][strlen(pathsarr[i])-1];
      if (last_char != PATH_SLASH) {
        snprintf(abs_file_path_buffer, buffer_size, "%s%c%s", pathsarr[i], PATH_SLASH, file_name);
        if (access(abs_file_path_buffer, X_OK) == 0) {
          return; 
        }
      } else {
        snprintf(abs_file_path_buffer, buffer_size, "%s%s", pathsarr[i], file_name);
        if (access(abs_file_path_buffer, X_OK) == 0) {
          return; 
        }
      }
    }
  }
  abs_file_path_buffer[0] = '\0';
}

int execute_program(char *executable_path, char **args) {
  int status;
  pid_t pid = fork();

  if (pid < 0) {
    perror("Fork failed when executing");
    return -1;
  } 
  else if (pid == 0) { //child process
    if (-1 == execv(executable_path, args)) {
      perror("failed execv");
      exit(1);
    }
  }
  else {
    while (0 == waitpid(pid, &status, 0)) {
      sleep(.1);
    }
  }
  return 0;
}


int main() {
  char executable_abs_path[MAX_PATH_LEN];
  executable_abs_path[0] = '\0';
  
  while (1) {
    printf("$ ");
    fflush(stdout);

    char input_orig[MAX_INPUT_LEN];
    char input[MAX_INPUT_LEN];
    fgets(input, 100, stdin);

    remove_trailing_newline(input);

    if (input[0] == '\0') {
      continue;
    }

    strcpy(input_orig, input);
    
    if (strncmp(input, "exit", 4) == 0) {
      char *exit_code_str;
      strtok(input, " ");
      exit_code_str = strtok(NULL, " ");
      int exit_code = atoi(exit_code_str);  // atoi returns 0 if an invalid string is returned
      return exit_code;

    } else if (strncmp(input, "echo", 4) == 0) {
      char* echoString = input; 
      echoString += 5; // add len("echo ") to pointer
      while (*echoString != '\0' && *echoString == ' ') {
        echoString += 1;
      }
      printf("%s\n", echoString);

    } else if (strncmp(input, "type", 4) == 0) {
      char *typeCommand;
      strtok(input, " ");
      typeCommand = strtok(NULL, " ");
      
      if (strcmp(typeCommand, "exit") == 0 
        || strcmp(typeCommand, "echo") == 0 
        || strcmp(typeCommand, "type") == 0) {
        printf("%s %s\n", typeCommand, MSG_BUILTIN);
      } else {
        get_file_from_path_env(typeCommand, executable_abs_path, MAX_PATH_LEN);
        if (executable_abs_path[0] == '\0') {
          printf("%s: not found\n", typeCommand);
        } else {
          printf("%s is %s\n", typeCommand, executable_abs_path);
        }
        executable_abs_path[0] = '\0';
      }

    } else {
      char* argv[MAX_ARGV_LEN];
      
      char* cmd = strtok(input, " ");

      get_file_from_path_env(cmd, executable_abs_path, MAX_PATH_LEN);
      if (executable_abs_path[0] == '\0') {
        printf("%s: command not found\n", cmd);
      } else {
        argv[0] = cmd;
        
        char* arg;
        int i = 1;
        while (NULL != (arg = strtok(NULL, " "))) {
          if (i < MAX_ARGV_LEN) {
            argv[i] = arg;
          }
          i++;
        }
        if (i < MAX_ARGV_LEN) {
          argv[i+1] = NULL;
          execute_program(executable_abs_path, argv);
        }
        else {
          printf("Max numer of arguments is %d, provided %d\n", MAX_ARGV_LEN, i);
        }
      }
      executable_abs_path[0] = '\0';
    }

    fflush(stdout);
  }
  
  return 0;
}