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
#include "goolib-mail.h"

static const char *const usages[] = 
{
  "goo-mail-latest [options]",
  NULL,
};

int main(int argc, char* argv[]) 
{
  char* imap_host = NULL;
  char* imap_port = NULL;
  char* imap_user = NULL;
  char* imap_password = NULL;
  char* temp_dir = NULL;
  char* download_dir = NULL;

  struct argparse_option options[] = {
    OPT_HELP(),
    OPT_STRING('s', "imap-host", &imap_host, "imap host domain", NULL, 0, 0),
    OPT_STRING('p', "imap-port", &imap_port, "imap port, 993 or 995", NULL, 0, 0),
    OPT_STRING('u', "imap-user", &imap_user, "imap account name", NULL, 0, 0),
    OPT_STRING('a', "imap-password", &imap_password, "imap account password", NULL, 0, 0),
    OPT_STRING('t', "temp-dir", &temp_dir, "temporary directory", NULL, 0, 0),
    OPT_STRING('d', "download-dir", &download_dir, "download directory", NULL, 0, 0),
    OPT_END(),
  };
  struct argparse argparse;
  argparse_init(&argparse, options, usages, 0);
  argparse_describe(&argparse, "\nSend mail.", NULL);
  
  argc = argparse_parse(&argparse, argc, (const char**) argv);
  if (imap_host == NULL || 
      imap_port == NULL || 
      imap_user == NULL || 
      imap_password == NULL || 
      temp_dir == NULL || 
      download_dir == NULL) 
  {
    argparse_usage(&argparse);
    return GOO_ERROR_FAILURE;
  }

  char* error = NULL;
  int rc = goo_mail_latest(imap_host, imap_port, 
                           imap_user, imap_password, 
                           temp_dir, download_dir, 
                           &error);
  if (rc != GOO_SUCCESS)
  {
    fprintf(stderr, "Error: %s\n", error);
    free(error);
    return GOO_ERROR_FAILURE;
  }
  return GOO_SUCCESS; 
}