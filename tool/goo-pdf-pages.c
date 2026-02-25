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
#include <dirent.h>
#include <errno.h>
#include <string.h>

#include <argparse.h>
#include <gfc.h>

#include "goolib-error.h"
#include "goolib-pdf.h"

static const char *const usages[] = 
{
  "goo-pdf-pages [options]",
  NULL,
};

int main(int argc, char *argv[]) 
{
  char* pdf_path = NULL;

  struct argparse_option options[] = {
    OPT_HELP(),
    OPT_STRING('p', "pdf", &pdf_path, "pdf path", NULL, 0, 0),
    OPT_END(),
  };

  struct argparse argparse;
  argparse_init(&argparse, options, usages, 0);
  argparse_describe(&argparse, "\nGet total page count of pdf.", NULL);
  
  argc = argparse_parse(&argparse, argc, (const char**) argv);
  if (pdf_path == NULL)
  {
    argparse_usage(&argparse);
    return GOO_ERROR_FAILURE;
  }

  int page_count = goo_pdf_pages(pdf_path);
  printf("%d", page_count);

  return GOO_SUCCESS; 
}
