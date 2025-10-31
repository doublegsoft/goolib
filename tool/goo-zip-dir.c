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
#include "goolib-zip.h"

static const char *const usages[] = {
    "goo-zip-dir [options]",
    NULL,
};

int main(int argc, char* argv[]) 
{
  char* dir = NULL;
  char* zip = NULL;

  struct argparse_option options[] = {
    OPT_HELP(),
    OPT_STRING('d', "dir", &dir, "directory to compress", NULL, 0, 0),
    OPT_STRING('z', "zip", &zip, "output zip file", NULL, 0, 0),
    OPT_END(),
  };
  struct argparse argparse;
  argparse_init(&argparse, options, usages, 0);
  argparse_describe(&argparse, "\nZip directory.", NULL);
  
  argc = argparse_parse(&argparse, argc, (const char**) argv);

  if (dir == NULL || zip == NULL)
  {
    argparse_usage(&argparse);
    return GOO_ERROR_FAILURE;
  }

  char* error = NULL;

  int rc = goo_zip_dir(dir, zip, &error);
  if (rc != GOO_SUCCESS)
  {
    fprintf(stderr, "Error: %s\n", error);
    free(error);
    return GOO_ERROR_FAILURE;
  }
  return GOO_SUCCESS; 
}