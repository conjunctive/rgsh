#define _POSIX_SOURCE

#include <regex.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_ERROR_MSG 4096
#define MAX_LINE 1000

int compile_regex(regex_t* regex, const char* text)
{
  int status = regcomp(regex, text, REG_NEWLINE);
  if (status != 0) {
    char error_message[MAX_ERROR_MSG];
    regerror(status, regex, error_message, MAX_ERROR_MSG);
    printf("Invalid regular expression '%s': %s\n", text, error_message);
    return 1;
  }
  return 0;
}

int match_line(regex_t* regex, regmatch_t* match, char* line, FILE* file)
{
  int found = 1;
  while (fgets(line, MAX_LINE, file) != NULL) {
    if (regexec(regex, line, 1, match, 0) == 0) {
      found = 0;
      break;
    }
  }
  return found;
}

int main(int argc, char** argv)
{
  int return_val = 0;
  regex_t regex;
  regmatch_t match;
  const char* regex_text;
  const char* on_match;
  char* line = (char*)malloc(MAX_LINE);
  char* match_text = (char*)malloc(MAX_LINE);

  pid_t pid;

  if (argc != 3) {
    printf("usage: rgsh regex command\n");
    return_val = 1;
    goto cleanup_malloc;
  }

  regex_text = argv[1];
  on_match = argv[2];

  if (compile_regex(&regex, regex_text) != 0) {
    return_val = 1;
    goto cleanup_all;
  }

  if (match_line(&regex, &match, line, stdin) != 0) {
    return_val = 1;
    goto cleanup_all;
  }

  if ((match.rm_so >= 0) && (match.rm_eo >= 1) && (match.rm_so != match.rm_eo)) {
    strncpy(match_text, line, match.rm_eo - match.rm_so);
    match_text[match.rm_eo - match.rm_so] = '\0';
  } else {
    return_val = 1;
    goto cleanup_all;
  }

  pid = fork();
  if (pid < 0) {
    fprintf(stderr, "Error occured: %d.\n", pid);
    return_val = -1;
    goto cleanup_all;
  } else if (pid == 0) {
    execlp("/bin/sh", "/bin/sh", "-c", on_match, match_text, NULL);
  } else {
    waitpid(pid, NULL, 0);
    goto cleanup_all;
  }

  return return_val;

 cleanup_malloc:
  free(line);
  free(match_text);
  return return_val;

 cleanup_all:
  regfree(&regex);
  free(line);
  free(match_text);
  return return_val;
}
