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
#include "goolib-util.h"


static const char *const usages[] = 
{
  "goo-png-color [options]",
  NULL,
};

int main(int argc, char* argv[]) 
{
  char* img = NULL;
  char* out = NULL;
  char* src_clr = NULL;
  char* dst_clr = NULL;

  struct argparse_option options[] = {
    OPT_HELP(),
    OPT_STRING('i', "image", &img, "the original image", NULL, 0, 0),
    OPT_STRING('o', "output", &out, "the output image", NULL, 0, 0),
    OPT_STRING('s', "source", &src_clr, "the source color", NULL, 0, 0),
    OPT_STRING('d', "destin", &dst_clr, "the destin color", NULL, 0, 0),
    OPT_END(),
  };
  struct argparse argparse;
  argparse_init(&argparse, options, usages, 0);
  argparse_describe(&argparse, "\nReplace a color in a png image.", NULL);
  
  argc = argparse_parse(&argparse, argc, (const char**) argv);
  if (img == NULL || out == NULL || src_clr == NULL || dst_clr == NULL) 
  {
    argparse_usage(&argparse);
    return GOO_ERROR_FAILURE;
  }

  int width, height, src_r, src_g, src_b, dst_r, dst_g, dst_b;
  unsigned char* rgba = goo_png_read(img, &width, &height);
  if (rgba == NULL) 
  {
    fprintf(stderr, "Failed to read PNG file: %s\n", img);
    return GOO_ERROR_FAILURE;
  }
  goo_color_hex2rgb(src_clr, &src_r, &src_g, &src_b);
  goo_color_hex2rgb(dst_clr, &dst_r, &dst_g, &dst_b);
  goo_png_color(rgba, width, height, src_r, src_g, src_b, dst_r, dst_g, dst_b);

  goo_png_write(out, rgba, width, height);

  free(rgba);

  return GOO_SUCCESS; 
}