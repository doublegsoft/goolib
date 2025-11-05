/*
** ─────────────────────────────────────────────────────────────────────────────────────────
** ─██████████████─██████████████─██████████████─██████─────────██████████─██████████████───
** ─██░░░░░░░░░░██─██░░░░░░░░░░██─██░░░░░░░░░░██─██░░██─────────██░░░░░░██─██░░░░░░░░░░██───
** ─██░░██████████─██░░██████░░██─██░░██████░░██─██░░██─────────████░░████─██░░██████░░██───
** ─██░░██─────────██░░██──██░░██─██░░██──██░░██─██░░██───────────██░░██───██░░██──██░░██───
** ─██░░██─────────██░░██──██░░██─██░░██──██░░██─██░░██───────────██░░██───██░░██████░░████─
** ─██░░██──██████─██░░██──██░░██─██░░██──██░░██─██░░██───────────██░░██───██░░░░░░░░░░░░██─
** ─██░░██──██░░██─██░░██──██░░██─██░░██──██░░██─██░░██───────────██░░██───██░░████████░░██─
** ─██░░██──██░░██─██░░██──██░░██─██░░██──██░░██─██░░██───────────██░░██───██░░██────██░░██─
** ─██░░██████░░██─██░░██████░░██─██░░██████░░██─██░░██████████─████░░████─██░░████████░░██─
** ─██░░░░░░░░░░██─██░░░░░░░░░░██─██░░░░░░░░░░██─██░░░░░░░░░░██─██░░░░░░██─██░░░░░░░░░░░░██─
** ─██████████████─██████████████─██████████████─██████████████─██████████─████████████████─
*/

#if defined(_WIN32) || defined(__CYGWIN__)
    /* Windows build will use the other implementation – see below */
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#endif

#include <stdio.h>

#include "goolib-error.h"
#include "goolib-proc.h"

typedef struct 
{
  int         exit_code;  
  char*       out_buf;
  char*       err_buf;
} goo_spawn_result_t;

typedef struct {
  int         fd;
  char**      buf;
} goo_thread_data_t;


static int 
goo_fd_to_buff(int fd, char** out_buf) 
{
  const size_t CHUNK = 8192;  
  size_t capacity = CHUNK;
  size_t len = 0;
  char buffer[CHUNK];

  if (*out_buf != NULL)
    len = strlen(*out_buf);

  while (1) {
    ssize_t r = read(fd, buffer, CHUNK);
    if (r <= 0) break;
    if (*out_buf == NULL)
      *out_buf = malloc(CHUNK);
    else if (capacity < len + (size_t)r) \
    {
      capacity *= 2;
      char *tmp = realloc(*out_buf, capacity);
      if (!tmp) return -1;
      *out_buf = tmp;
    }

    memcpy(*out_buf + len, buffer, r);
    len += r;
    (*out_buf)[len] = '\0';
  }
  return 0;
}

static void*
goo_thread_reader(void* arg) {
  goo_thread_data_t* td = (goo_thread_data_t *)arg;
  goo_fd_to_buff(td->fd, td->buf);
  return NULL;
}

int
goo_proc_exec(const char* command, char** out, char** err) 
{
  int stdout_pipe[2];
  int stderr_pipe[2];
  goo_spawn_result_t r = { .exit_code = -1, .out_buf = NULL, .err_buf = NULL };

  if (pipe(stdout_pipe) || pipe(stderr_pipe))
    return GOO_ERROR_FAILURE;

  pid_t pid = fork();
  if (pid == -1) {
    close(stdout_pipe[0]); close(stdout_pipe[1]);
    close(stderr_pipe[0]); close(stderr_pipe[1]);
    return GOO_ERROR_FAILURE;
  }

  if (pid == 0) {                      
    close(stdout_pipe[0]); close(stderr_pipe[0]);
    dup2(stdout_pipe[1], STDOUT_FILENO); 
    dup2(stderr_pipe[1], STDERR_FILENO);
    close(stdout_pipe[1]); close(stderr_pipe[1]);

    execlp("/bin/sh", "sh", "-c", command, (char *)NULL);
    _exit(127);                    
  }

  close(stdout_pipe[1]); close(stderr_pipe[1]);

  goo_thread_data_t td_out = { .fd = stdout_pipe[0], .buf = &r.out_buf };
  goo_thread_data_t td_err = { .fd = stderr_pipe[0], .buf = &r.err_buf };

  pthread_t t_out, t_err;
  pthread_create(&t_out, NULL, goo_thread_reader, &td_out);
  pthread_create(&t_err, NULL, goo_thread_reader, &td_err);

  pthread_join(t_out, NULL);
  pthread_join(t_err, NULL);

  waitpid(pid, &r.exit_code, 0);

  close(stdout_pipe[0]); 
  close(stderr_pipe[0]);

  *out = r.out_buf;
  *err = r.err_buf;
  return GOO_SUCCESS;
} 
