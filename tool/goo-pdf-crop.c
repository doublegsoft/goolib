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

#define PDFTOIMG "/Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/3rd/xpdf-4.05/build/darwin/xpdf/pdftopng"

typedef struct rect_s
{
  int x0;
  int y0;
  int x1;
  int y1;
  int w;
  int h;
} 
rect_t;

static const char *const usages[] = 
{
  "goo-pdf-crop [options]",
  NULL,
};

static void
goo_pdf_dir(const char* path)
{
  DIR* dir = opendir(path);
  if (!dir) {
    fprintf(stderr, "opendir(%s) failed: %s\n", path, strerror(errno));
    return;
  }

  struct dirent* entry;
  while ((entry = readdir(dir)) != NULL) 
  {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;
    if (strstr(entry->d_name, "_ENG") != NULL)
      continue;
    char cmd[8192] = {'\0'};
    char id[13] = {'\0'};
    memcpy(id, entry->d_name, 12);
    id[12] = '\0';
    sprintf(cmd, PDFTOIMG " %s/%s %s/../pdf_img/%s", 
      path, entry->d_name, path, id);
    char* out = NULL;
    char* err = NULL;
    int rc = goo_proc_exec(cmd, &out, &err);
    if (err != NULL)
    {
      free(err);
    }
    if (out != NULL)
    {
      free(out);
    }
  }
  closedir(dir);

  char pdf_img_dir[8192];
  sprintf(pdf_img_dir, "%s/../pdf_img", path);
  dir = opendir(pdf_img_dir);

  while ((entry = readdir(dir)) != NULL) 
  {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;
    char full_path[8192] = {'\0'};
    strcpy(full_path, pdf_img_dir);
    strcat(full_path, "/");
    strcat(full_path, entry->d_name);

    char id[13] = {'\0'};
    memcpy(id, entry->d_name, 12);
    id[12] = '\0';

    char* error = NULL;
    int index = entry->d_name[18] - '0';
    char out_path[8192] = {'\0'};
    sprintf(out_path, "%s/../pdf_crop/%s_bitems_%d.png", path, id, index);
    goo_png_crop(full_path, 40, 860, 1120, 420, out_path, &error);
    if (error != NULL)
      free(error);
    sprintf(out_path, "%s/../pdf_crop/%s_primary_%d.png", path, id, index);
    goo_png_crop(full_path, 828, 318, 400, 550, out_path, &error);
    if (error != NULL)
      free(error);
    sprintf(out_path, "%s/../pdf_crop/%s_consumption_%d.png", path, id, index);
    goo_png_crop(full_path, 75, 330, 700, 455, out_path, &error);
    if (error != NULL)
      free(error);
    sprintf(out_path, "%s/../pdf_crop/%s_footer_%d.png", path, id, index);
    goo_png_crop(full_path, 70, 1334, 1150, 350, out_path, &error);
    if (error != NULL)
      free(error);
  }
  closedir(dir);
}

int main(int argc, char *argv[]) 
{
  gfc_gc_init();

  char* coords = NULL;
  char* pdf = NULL;
  char* dir = NULL;
  char* error = NULL;

  struct argparse_option options[] = {
    OPT_HELP(),
    OPT_STRING('d', "dir", &dir, "pdf directory", NULL, 0, 0),
    OPT_STRING('p', "pdf", &pdf, "pdf file path", NULL, 0, 0),
    OPT_STRING('c', "coords", &coords, "coordination expression", NULL, 0, 0),
    OPT_END(),
  };
  struct argparse argparse;
  argparse_init(&argparse, options, usages, 0);
  argparse_describe(&argparse, "\nCrop pdf to images.", NULL);
  
  argc = argparse_parse(&argparse, argc, (const char**) argv);
  if (coords == NULL || 
      (pdf == NULL && dir == NULL)) 
  {
    argparse_usage(&argparse);
    return GOO_ERROR_FAILURE;
  }

  cJSON* json = cJSON_Parse(coords);
  if (json == NULL)
  {
    fprintf(stderr, "Invalid JSON\n");
    return GOO_ERROR_FAILURE;
  }

  if (dir != NULL)
  {
    goo_pdf_dir(dir);
    return GOO_SUCCESS;
  }

  gfc_list_p rects = gfc_list_new();
  gfc_list_free(rects);
  return GOO_SUCCESS; 
}
