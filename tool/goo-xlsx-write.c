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
  "goo-xlsx-write [options]",
  NULL,
};

int 
main(int argc, char* argv[]) 
{
  char* file_path = NULL;
  char* sheet_name = NULL;
  char* data = NULL;
  int row = 0;
  int col = 0;

  struct argparse_option options[] = {
    OPT_HELP(),
    OPT_STRING('f', "file", &file_path, "the file path", NULL, 0, 0),
    OPT_STRING('s', "sheet", &sheet_name, "the sheet name", NULL, 0, 0),
    OPT_INTEGER('r', "row", &row, "the row index", NULL, 0, 0),
    OPT_INTEGER('c', "column", &col, "the column index", NULL, 0, 0),
    OPT_STRING('d', "data", &data, "the data", NULL, 0, 0),
    OPT_END(),
  };
  
  struct argparse argparse;
  argparse_init(&argparse, options, usages, 0);
  argparse_describe(&argparse, "\nWrite data to XLSX file.", NULL);
  
  argc = argparse_parse(&argparse, argc, (const char**) argv);

  if (sheet_name == NULL)
    sheet_name = "Sheet1";

  if (file_path == NULL || 
      data == NULL) 
  {
    argparse_usage(&argparse);
    return GOO_ERROR_FAILURE;
  }

  goo_xlsx_write(file_path, sheet_name, row, col, data);
  return GOO_SUCCESS; 
}
