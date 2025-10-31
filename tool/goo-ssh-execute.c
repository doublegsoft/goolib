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
#include <stdio.h>
#include <stdlib.h>
#include <argparse.h>

#include "goolib-error.h"
#include "goolib-ssh.h"

static const char *const usages[] = 
{
  "goo-ssh-execute [options]",
  NULL,
};

int main(int argc, char* argv[]) 
{
  char* host = NULL;
  int port = 0;
  char* username = NULL;
  char* password = NULL;
  char* command = NULL;

  struct argparse_option options[] = {
    OPT_HELP(),
    OPT_STRING('s', "server", &host, "ssh server address", NULL, 0, 0),
    OPT_INTEGER('p', "port", &port, "ssh port, 22", NULL, 0, 0),
    OPT_STRING('u', "username", &username, "account name", NULL, 0, 0),
    OPT_STRING('a', "password", &password, "account password", NULL, 0, 0),
    OPT_STRING('c', "command", &command, "executing command", NULL, 0, 0),
    OPT_END(),
  };

  struct argparse argparse;
  argparse_init(&argparse, options, usages, 0);
  argparse_describe(&argparse, "\nUpload file via SFTP.", NULL);
  
  argc = argparse_parse(&argparse, argc, (const char**) argv);

  if (host == NULL || 
      port == 0 || 
      username == NULL || 
      password == NULL || 
      command == NULL) 
  {
    argparse_usage(&argparse);
    return GOO_ERROR_FAILURE;
  }

  char* error = NULL;
  int rc = goo_ssh_execute(host, port, 
                           username, password, 
                           command,
                           &error);
  if (rc != GOO_SUCCESS)
  {
    fprintf(stderr, "Error: %s\n", error);
    if (error != NULL)
      free(error);
    return GOO_ERROR_FAILURE;
  }
  return GOO_SUCCESS; 
}
