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
  "goo-png-concat [options]",
  NULL,
};

int main(int argc, char** argv) 
{
  char* left = NULL;
  char* right = NULL;
  char* out = NULL;

  struct argparse_option options[] = {
    OPT_HELP(),
    OPT_STRING('l', "left", &left, "the left image", NULL, 0, 0),
    OPT_STRING('r', "right", &right, "the right image", NULL, 0, 0),
    OPT_STRING('o', "output", &out, "the output image", NULL, 0, 0),
    OPT_END(),
  };
  struct argparse argparse;
  argparse_init(&argparse, options, usages, 0);
  argparse_describe(&argparse, "\nReplace a color in a png image.", NULL);
  
  argc = argparse_parse(&argparse, argc, (const char**) argv);
  if (left == NULL || right == NULL || out == NULL) 
  {
    argparse_usage(&argparse);
    return GOO_ERROR_FAILURE;
  }

  // int w,h,c;
  // unsigned char* png_data = goo_png_read("/Users/christian/Downloads/000240276393G.png", &w, &h, &c);
  // goo_png_write("/Users/christian/Downloads/a.png", png_data, w, h, c);

  // if (1) return 1;
  int l_w, l_h, l_c, r_w, r_h, r_c;
  unsigned char* left_img = goo_png_read(left, &l_w, &l_h, &l_c);
  unsigned char* right_img = goo_png_read(right, &r_w, &r_h, &r_c);

  if (!left_img || !right_img) {
    fprintf(stderr, "Error reading input images.\n");
    return 1;
  }

  int out_w, out_h;
  unsigned char* result = goo_png_concat(left_img, l_w, l_h, l_c, right_img, r_w, r_h, r_c, &out_w, &out_h);

  if (result) {
    goo_png_write(out, result, out_w, out_h, 4);
    free(result);
  }

  free(left_img);
  free(right_img);
  return 0;
}