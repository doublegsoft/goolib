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
#include <string.h>

#include "goolib-error.h"
#include "goolib-xlsx.h"

static const char *const usages[] = 
{
  "goo-xlsx-replace [options]",
  NULL,
};

int 
main(int argc, char* argv[]) 
{
  char* file_path = NULL;
  char* sheet_name = NULL;
  char* data = NULL;
  char* key = NULL;
  int idx = 1;
  int col = 1;

  struct argparse_option options[] = {
    OPT_HELP(),
    OPT_STRING('f', "file", &file_path, "the file path", NULL, 0, 0),
    OPT_STRING('s', "sheet", &sheet_name, "the sheet name", NULL, 0, 0),
    OPT_STRING('k', "key", &key, "the key value", NULL, 0, 0),
    OPT_INTEGER('i', "index", &idx, "the key column index", NULL, 0, 0),
    OPT_INTEGER('c', "column", &col, "the column index", NULL, 0, 0),
    OPT_STRING('d', "data", &data, "the data", NULL, 0, 0),
    OPT_END(),
  };
  
  struct argparse argparse;
  argparse_init(&argparse, options, usages, 0);
  argparse_describe(&argparse, "\nReplace or write data to XLSX file.", NULL);
  
  argc = argparse_parse(&argparse, argc, (const char**) argv);

  if (sheet_name == NULL)
    sheet_name = "Sheet1";

  if (file_path == NULL || 
      data == NULL) 
  {
    argparse_usage(&argparse);
    return GOO_ERROR_FAILURE;
  }

  goo_xlsx_replace(file_path, sheet_name, key, idx, col, data);
  return GOO_SUCCESS; 
}
