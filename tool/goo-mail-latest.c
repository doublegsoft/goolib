/*
**                                ▄▄▄▄         ██     ▄▄       
**                                ▀▀██         ▀▀     ██       
**   ▄███▄██   ▄████▄    ▄████▄     ██       ████     ██▄███▄  
**  ██▀  ▀██  ██▀  ▀██  ██▀  ▀██    ██         ██     ██▀  ▀██ 
**  ██    ██  ██    ██  ██    ██    ██         ██     ██    ██ 
**  ▀██▄▄███  ▀██▄▄██▀  ▀██▄▄██▀    ██▄▄▄   ▄▄▄██▄▄▄  ███▄▄██▀ 
**   ▄▀▀▀ ██    ▀▀▀▀      ▀▀▀▀       ▀▀▀▀   ▀▀▀▀▀▀▀▀  ▀▀ ▀▀▀   
**   ▀████▀▀ 
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
  char* imap_host = NULL;
  char* imap_port = NULL;
  char* imap_user = NULL;
  char* imap_password = NULL;
  char* mail_body = NULL;
  char* tmp_dir = NULL;
  char* dl_dir = NULL;

  struct argparse_option options[] = {
    OPT_HELP(),
    OPT_STRING('s', "imap-host", &imap_host, "IMAP host domain", NULL, 0, 0),
    OPT_STRING('p', "imap-port", &imap_port, "IMAP port, 993", NULL, 0, 0),
    OPT_STRING('u', "imap-user", &imap_user, "IMAP account name", NULL, 0, 0),
    OPT_STRING('a', "imap-password", &imap_password, "IMAP account password", NULL, 0, 0),
    OPT_STRING('t', "temporary-directory", &tmp_dir, "the temporary directory", NULL, 0, 0),
    OPT_STRING('d', "download-directory", &dl_dir, "the download directory", NULL, 0, 0),
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
      tmp_dir == NULL || 
      dl_dir == NULL) 
  {
    argparse_usage(&argparse);
    return GOO_ERROR_FAILURE;
  }

  char* error = NULL;
  int rc = goo_mail_latest(imap_host, imap_port, 
                           imap_user, imap_password, 
                           tmp_dir, dl_dir, 
                           &mail_body, 
                           &error);
  if (rc > 0)
    goo_mail_read(imap_host, imap_port, imap_user, imap_password, rc, NULL);
  return GOO_SUCCESS; 
}