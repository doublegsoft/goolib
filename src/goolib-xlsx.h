/*
**                                ▄▄▄▄         ██     ▄▄       
**                                ▀▀██         ▀▀     ██       
**   ▄███▄██   ▄████▄    ▄████▄     ██       ████     ██▄███▄  
**  ██▀  ▀██  ██▀  ▀██  ██▀  ▀██    ██         ██     ██▀  ▀██ 
**  ██    ██  ██    ██  ██    ██    ██         ██     ██    ██ 
**  ▀██▄▄███  ▀██▄▄██▀  ▀██▄▄██▀    ██▄▄▄   ▄▄▄██▄▄▄  ███▄▄██▀ 
**   ▄▀▀▀ ██    ▀▀▀▀      ▀▀▀▀       ▀▀▀▀   ▀▀▀▀▀▀▀▀  ▀▀ ▀▀▀   
**   ▀████▀▀ 
*/ 

typedef struct goo_xlsx_style_s
{
  char bgcolor[12];
  char fgcolor[12];
} goo_xlsx_style_t;

/*!
** Writes a string value to a specific cell in an Excel file.
**
** If the file exists, it opens it; otherwise, it creates a new file.
**
** @param file_path  Path to the .xlsx file
** @param sheet_name Name of the target worksheet (e.g., "Sheet1")
** @param row        Row index (1-based)
** @param col        Column index (1-based)
** @param data       String data to write into the cell
** @return           GOO_SUCCESS on success, GOO_ERROR_FAILURE on error
*/
int
goo_xlsx_write(const char*          file_path, 
               const char*          sheet_name,
               int                  row, 
               int                  col,
               char*                data,
               goo_xlsx_style_t*    style);

int
goo_xlsx_replace(const char*        file_path, 
                 const char*        sheet_name,
                 const char*        key,
                 int                key_index, 
                 int                col,
                 char*              data,
                 goo_xlsx_style_t*  style);               

int
goo_xlsx_read(const char*       file, 
              char*             data, 
              int*              rows, 
              int*              cols);