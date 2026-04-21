#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

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

    if (strcmp(args[0], "exit") == 0) {
      exit(0);
    }

    free(line);

    pid_t pid = fork();

    if (pid == 0) {
      execlp(args[0], args[0], args[1], args[2], NULL);
      exit(1);
    } else if (pid > 0) {
      waitpid(pid, NULL, 0);
    } else {
      perror("fork faild");
    }
  }
  return 0;
}
