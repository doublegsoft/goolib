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

#include <gfc.h>

#include "goolib-png.h"
#include "goolib-error.h"

static const char *const usages[] = 
{
  "goo-png-grayscale [options]",
  NULL,
};

int main(int argc, char* argv[]) 
{
  char* img = NULL;
  char* out = NULL;

  struct argparse_option options[] = {
    OPT_HELP(),
    OPT_STRING('i', "image", &img, "the original image", NULL, 0, 0),
    OPT_STRING('o', "output", &out, "the output image", NULL, 0, 0),
    OPT_END(),
  };
  struct argparse argparse;
  argparse_init(&argparse, options, usages, 0);
  argparse_describe(&argparse, "\nGrayscale a png image.", NULL);
  
  argc = argparse_parse(&argparse, argc, (const char**) argv);
  if (img == NULL || out == NULL) 
  {
    argparse_usage(&argparse);
    return GOO_ERROR_FAILURE;
  }

  int width, height;
  unsigned char* rgba = goo_png_read(img, &width, &height);
  if (rgba == NULL) {
    fprintf(stderr, "Failed to read PNG file: %s\n", img);
    return GOO_ERROR_FAILURE;
  }
  unsigned char* gray = goo_png_grayscale(rgba, width, height);
  if (gray == NULL) {
    fprintf(stderr, "Failed to convert PNG to grayscale: %s\n", img);
    free(rgba);
    return GOO_ERROR_FAILURE;
  }

  goo_png_write(out, gray, width, height);

  free(rgba);
  free(gray);

  return GOO_SUCCESS; 
}