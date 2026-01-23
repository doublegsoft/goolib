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
#include <OpenXLSX.hpp>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

using namespace OpenXLSX;
using namespace std;
namespace fs = std::filesystem;

#include "goolib-xlsx.h"
#include "goolib-error.h"

int
goo_xlsx_write(const char* file_path, 
               const char* sheet_name,
               int row, 
               int col,
               char* data,
               goo_xlsx_style_t* style)
{
  XLDocument doc;
  try {
    if (fs::exists(file_path)) {
      doc.open(file_path);
    } else {
      doc.create(file_path);
    }
  }
  catch (const std::exception& e) {
    return GOO_ERROR_FAILURE;
  }
  auto wks = doc.workbook().worksheet(sheet_name);
  wks.cell(row, col).value() = data;
  doc.save();
  doc.close();
  return GOO_SUCCESS;
}

/*!
** Searches for a key in a specific column and updates a value in another column.
**
** - If the file exists, it is opened; otherwise, a new file is created.
** - It iterates through the rows searching for the 'key' in the 'key_index' column.
** - If the key is found, the loop breaks, and the data is written to that row.
** - If an empty cell is encountered in the key column before finding the key, 
**   it stops there and writes the data (effectively appending).
**
** @param file_path  Path to the .xlsx file
** @param sheet_name Name of the target worksheet
** @param key        The string key to search for
** @param key_index  The column index to search the key in (1-based)
** @param col        The target column index to write the new data (1-based)
** @param data       The data string to write
** @return           GOO_SUCCESS on success, GOO_ERROR_FAILURE on error
*/
int
goo_xlsx_replace(const char* file_path, 
                 const char* sheet_name,
                 const char* key,
                 int key_index, 
                 int col,
                 char* data,
                 goo_xlsx_style_t* style)
{
  int row_idx = 1;
  XLDocument doc;
  try {
    if (fs::exists(file_path)) {
      doc.open(file_path);
    } else {
      doc.create(file_path);
    }
  }
  catch (const std::exception& e) {
    return GOO_ERROR_FAILURE;
  }
  auto wks = doc.workbook().worksheet(sheet_name);

  string target(key);
  for (auto& row : wks.rows()) {
    string s = wks.cell(row_idx, key_index).value().get<std::string>();
    if (s == target) {
      break;
    }
    row_idx++;
    XLCellValue val = wks.cell(row_idx, key_index).value();
    if (val.type() == XLValueType::Empty) {
      break;
    }
  }

  auto cell = wks.cell(row_idx, col);

  if (style != NULL) 
  {
    XLCellFormats& cellFormats = doc.styles().cellFormats();
    XLStyleIndex cellFormat = cellFormats.create();
    XLFonts& fonts = doc.styles().fonts();
    if (strlen(style->fgcolor) != 0) {
      XLStyleIndex font = fonts.create();
	    fonts[font].setFontColor(XLColor(style->fgcolor));
      cellFormats[cellFormat].setFontIndex(font);
      cell.setCellFormat(cellFormat);
    }
  } 

  cell.value() = data;
  doc.save();
  doc.close();
  return GOO_SUCCESS;
}