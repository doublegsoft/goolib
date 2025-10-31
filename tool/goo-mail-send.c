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
  "goo-mail-send [options]",
  NULL,
};

int main(int argc, char* argv[]) 
{
  char* smtp_host = NULL;
  char* smtp_port = NULL;
  char* smtp_user = NULL;
  char* smtp_password = NULL;
  char* mail_from = NULL;
  char* mail_to = NULL;
  char* mail_subject = NULL; 
  char* mail_body = NULL;
  char* mail_file = NULL;

  struct argparse_option options[] = {
    OPT_HELP(),
    OPT_STRING('s', "smtp-host", &smtp_host, "SMTP host domain", NULL, 0, 0),
    OPT_STRING('p', "smtp-port", &smtp_port, "SMTP port, 465 or 587", NULL, 0, 0),
    OPT_STRING('u', "smtp-user", &smtp_user, "SMTP account name", NULL, 0, 0),
    OPT_STRING('a', "smtp-password", &smtp_password, "SMTP account password", NULL, 0, 0),
    OPT_STRING('f', "mail-from", &mail_from, "Sender email address", NULL, 0, 0),
    OPT_STRING('t', "mail-to", &mail_to, "Receiver email address", NULL, 0, 0),
    OPT_STRING('j', "mail-subject", &mail_subject, "Mail subject", NULL, 0, 0),
    OPT_STRING('b', "mail-body", &mail_body, "Mail content, html format", NULL, 0, 0),
    OPT_STRING('m', "mail-file", &mail_file, "Mail attachment", NULL, 0, 0),
    OPT_END(),
  };
  struct argparse argparse;
  argparse_init(&argparse, options, usages, 0);
  argparse_describe(&argparse, "\nSend mail.", NULL);
  
  argc = argparse_parse(&argparse, argc, (const char**) argv);
  if (smtp_host == NULL || 
      smtp_port == NULL || 
      smtp_user == NULL || 
      smtp_password == NULL || 
      mail_from == NULL || 
      mail_to == NULL || 
      mail_subject == NULL) 
  {
    argparse_usage(&argparse);
    return GOO_ERROR_FAILURE;
  }

  char* error = NULL;
  int rc = goo_mail_send(smtp_host, smtp_port, 
                         smtp_user, smtp_password, 
                         mail_from, mail_to, 
                         mail_subject == NULL ? "" : mail_subject, 
                         mail_body == NULL ? "" : mail_body, mail_file, 
                         &error);
  if (rc != GOO_SUCCESS)
  {
    fprintf(stderr, "Error: %s\n", error);
    free(error);
    return GOO_ERROR_FAILURE;
  }
  return GOO_SUCCESS; 
}