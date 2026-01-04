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
#include <dirent.h>
#include <errno.h>
#include <string.h>

#include <argparse.h>
#include <cJSON.h>
#include <cJSON_Utils.h>
#include <gfc.h>

#include "goolib-error.h"
#include "goolib-proc.h"
#include "goolib-pdf.h"

static const char *const usages[] = 
{
  "goo-pdf-text [options]",
  NULL,
};

int main(int argc, char *argv[]) 
{
  char* pdf_path = NULL;
  char* rect = NULL;
  int page_number = 1;
  char* output = NULL;

  struct argparse_option options[] = {
    OPT_HELP(),
    OPT_STRING('p', "png", &pdf_path, "pdf path", NULL, 0, 0),
    OPT_INTEGER('n', "page number", &page_number, "page number", NULL, 0, 0),
    OPT_STRING('r', "rect", &rect, "rect coordinates, xyxy format", NULL, 0, 0),
    OPT_END(),
  };

  struct argparse argparse;
  argparse_init(&argparse, options, usages, 0);
  argparse_describe(&argparse, "\nGet text from specific rect in pdf.", NULL);
  
  argc = argparse_parse(&argparse, argc, (const char**) argv);
  if (pdf_path == NULL || rect == NULL)
  {
    argparse_usage(&argparse);
    return GOO_ERROR_FAILURE;
  }

  double x1, y1, x2, y2;
  sscanf(rect, "%lf,%lf,%lf,%lf", &x1, &y1, &x2, &y2);
  

  char* error = NULL;
  char* text = NULL;
  goo_pdf_text(pdf_path, page_number, (int)x1, (int)y1, (int)(x2 - x1), (int)(y2 - y1), &text, &error);

  if (error != NULL)
  { 
    fprintf(stderr, "Error: %s\n", error);
    return GOO_ERROR_FAILURE;
  }

  printf("%s\n", text);

  return GOO_SUCCESS; 
}
