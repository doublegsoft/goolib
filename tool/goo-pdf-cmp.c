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
#include "goolib-ocr.h"
#include "goolib-pdf.h"

static const char *const usages[] = 
{
  "goo-pdf-cmp [options]",
  NULL,
};

int main(int argc, char *argv[]) 
{
  char* p1 = NULL;
  char* p2 = NULL;
  char* rect = NULL;
  int page_number = 1;
  char* output = NULL;

  // 初始化 tesseract
  goo_ocr_init("/Users/christian/export/local/models/tessdata", "eng+chi_tra+chi_sim");

  struct argparse_option options[] = {
    OPT_HELP(),
    OPT_STRING('f', "first", &p1, "the first pdf path", NULL, 0, 0),
    OPT_STRING('s', "second", &p2, "the second pdf path", NULL, 0, 0),
    OPT_INTEGER('n', "page number", &page_number, "page number", NULL, 0, 0),
    OPT_STRING('r', "rect", &rect, "rect coordinates, xyxy format", NULL, 0, 0),
    OPT_END(),
  };

  struct argparse argparse;
  argparse_init(&argparse, options, usages, 0);
  argparse_describe(&argparse, "\nGet text from specific rect in pdf.", NULL);
  
  argc = argparse_parse(&argparse, argc, (const char**) argv);
  if (p1 == NULL || p2 == NULL || rect == NULL)
  {
    argparse_usage(&argparse);
    return GOO_ERROR_FAILURE;
  }

  double x1, y1, x2, y2;
  int c;
  sscanf(rect, "%lf,%lf,%lf,%lf", &x1, &y1, &x2, &y2);

  char* error = NULL;
  char* text = NULL;

  unsigned char* img1 = NULL;
  unsigned char* img2 = NULL;

  goo_pdf_png(p1, page_number, (int)x1, (int)y1, (int)(x2 - x1), (int)(y2 - y1), &img1, &c, &error);
  goo_pdf_png(p2, page_number, (int)x1, (int)y1, (int)(x2 - x1), (int)(y2 - y1), &img2, &c, &error);

  int rc = goo_ocr_eq(img1, (int)(x2 - x1), (int)(y2 - y1), c, img2, (int)(x2 - x1), (int)(y2 - y1), c);

  printf("%d", rc);

  return GOO_SUCCESS; 
}
