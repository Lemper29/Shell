#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <unistd.h>

typedef enum { CMD_CD, CMD_EXTERNAL, CMD_EXIT } cmd_type_t;

cmd_type_t dispatcher_handler(char **args) {
  if (strcmp(args[0], "exit") == 0)
    return CMD_EXIT;

  if (strcmp(args[0], "cd") == 0) {
    return CMD_CD;
  }

  return CMD_EXTERNAL;
}

void cmd_exit(char **args) {
  if (strcmp(args[0], "exit") == 0) {
    exit(0);
  }
}

void cmd_cd(char **args) {
  if (strcmp(args[0], "cd") == 0) {
    if (chdir(args[1]) == -1) {
      perror(NULL);
    }
  }
}

int parse_comand(char *line, char **args, int max_int) {
  if (line == 0)
    return 0;

  char *copy = strdup(line);
  if (copy == NULL)
    return 0;

  int i = 0;
  char *token = strtok(copy, " \n");

  while (token != NULL && i < max_int) {
    args[i] = strdup(token);
    i++;
    token = strtok(NULL, " \t\n");
  }
  args[i] = NULL;

  free(copy);
  return i;
}

char *read_str() {
  char s[100];
  struct utsname buffer;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  if (uname(&buffer) < 0) {
    exit(1);
  }
  printf("%s@%s: %s -> ", getenv("LOGNAME"), buffer.nodename,
         getcwd(s, sizeof(s)));
  if ((read = getline(&line, &len, stdin)) == -1) {
    free(line);
    return NULL;
  }

  if (line[read - 1] == '\n') {
    line[read - 1] = '\0';
  }

  return line;
};

int main() {
  while (1) {
    char *line = read_str();
    char *args[20];

    parse_comand(line, args, 10);
    cmd_type_t dispatcher = dispatcher_handler(args);

    switch (dispatcher) {
    case CMD_EXIT:
      free(line);
      exit(0);
    case CMD_CD:
      cmd_cd(args);
      free(line);
      continue;
    case CMD_EXTERNAL:
      break;
    }

    free(line);

    pid_t pid = fork();

    if (pid == 0) {
      execvp(args[0], args);
      exit(1);
    } else if (pid > 0) {
      waitpid(pid, NULL, 0);
    } else {
      perror("fork faild");
    }
  }
  return 0;
}
