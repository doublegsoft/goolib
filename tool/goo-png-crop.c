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
#include "goolib-png.h"

static const char *const usages[] = 
{
  "goo-png-crop [options]",
  NULL,
};

int main(int argc, char *argv[]) 
{
  char* pngimg = NULL;
  char* rect = NULL;
  char* output = NULL;

  struct argparse_option options[] = {
    OPT_HELP(),
    OPT_STRING('p', "png", &pngimg, "png image to crop", NULL, 0, 0),
    OPT_STRING('r', "rect", &rect, "rect coordinates, xyxy format", NULL, 0, 0),
    OPT_STRING('o', "output", &output, "output image", NULL, 0, 0),
    OPT_END(),
  };

  struct argparse argparse;
  argparse_init(&argparse, options, usages, 0);
  argparse_describe(&argparse, "\nCrop png image.", NULL);
  
  argc = argparse_parse(&argparse, argc, (const char**) argv);
  if (pngimg == NULL || rect == NULL && output == NULL)
  {
    argparse_usage(&argparse);
    return GOO_ERROR_FAILURE;
  }

  double x1, y1, x2, y2;
  sscanf(rect, "%lf,%lf,%lf,%lf", &x1, &y1, &x2, &y2);

  char* error = NULL;
  goo_png_crop(pngimg, (int)x1, (int)y1, (int)(x2 - x1), (int)(y2 - y1), output, &error);

  if (error != NULL)
  { 
    fprintf(stderr, "Error: %s\n", error);
    return GOO_ERROR_FAILURE;
  }

  return GOO_SUCCESS; 
}
