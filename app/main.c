#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> 

static const char CMD_EXIT[] = "exit";
static const char CMD_ECHO[] = "echo";
static const char CMD_TYPE[] = "type";

static const char MSG_BUILTIN[] = "is a shell builtin";

static const char PATH_SEP = ':';
static const char PATH_SLASH = '/';

static const int MAX_PATH_LEN = 260;


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


int find_file_in_dir(char* dir_name, char *file_name) {
  DIR *d;
  struct dirent *dir;
  d = opendir(dir_name);
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if (strcmp(file_name, dir->d_name) == 0) {
        return 0;
      }
    }
    closedir(d);
  }
  return -1;
}


void get_file_from_path_env(char* file_name, char* abs_file_path_buffer, size_t buffer_size) {
  const char* path_original = getenv("PATH");

  // get number of paths in path env variable and path env variable length
  int n_paths = 0;
  size_t i = 0;
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
    if (pathsarr[i][0] != '\0' && find_file_in_dir(pathsarr[i], file_name) == 0) {

      char last_char = pathsarr[i][strlen(pathsarr[i])-1];
      if (last_char != PATH_SLASH) {
        snprintf(abs_file_path_buffer, buffer_size, "%s%c%s", pathsarr[i], PATH_SLASH, file_name);
        return;
      } else {
        snprintf(abs_file_path_buffer, buffer_size, "%s%s", pathsarr[i], file_name);
        return;
      }

    }
  }
}

int main() {
  char executable_abs_path[MAX_PATH_LEN];
  executable_abs_path[0] = '\0';
  
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
        get_file_from_path_env(typeCommand, executable_abs_path, MAX_PATH_LEN);
        if (executable_abs_path[0] == '\0') {
          printf("%s: not found\n", typeCommand);
        } else {
          printf("%s is %s\n", typeCommand, executable_abs_path);
        }
        executable_abs_path[0] = '\0';
      }

    } else {
        printf("%s: command not found\n", input);
        fflush(stdout);
    }
  }
  
  return 0;
}