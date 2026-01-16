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
#ifndef __GOOLIB_PDF_H__
#define __GOOLIB_PDF_H__

#ifdef __cplusplus
extern "C" {
#endif    

/*!
** Extracts text from a specific rectangular region of a PDF page using OCR.
**
** This function renders a specific slice of a PDF page to an in-memory image
** (at 300 DPI) and passes that image to Tesseract to extract the text.
**
** @param pdf_path Path to the input PDF file
** @param page     Page number (1-based)
** @param x        X coordinate of the crop (in PDF points)
** @param y        Y coordinate of the crop (in PDF points)
** @param w        Width of the crop (in PDF points)
** @param h        Height of the crop (in PDF points)
** @param text     Output: Pointer to hold the result string (must be freed by caller)
** @param error    Output: Pointer to hold error message if failure occurs
** @return         GOO_SUCCESS on success, GOO_ERROR_FAILURE on error
*/
int
goo_pdf_text(const char* pdf_path, 
             int page, 
             int x, 
             int y, 
             int w, 
             int h,
             char** text,
             char** error);

/*!
** Renders a rectangular region of a PDF page to a raw pixel buffer.
**
** This function uses Xpdf/Poppler to render a specific slice of the page
** at 300 DPI. It returns the raw raster data (RGB or Gray), not an 
** encoded PNG file.
**
** @param pdf_path Path to the input PDF file
** @param page     Page number (1-based)
** @param x        Crop X origin (in PDF Points)
** @param y        Crop Y origin (in PDF Points)
** @param w        Crop Width (in PDF Points)
** @param h        Crop Height (in PDF Points)
** @param png_data Output: Pointer to raw pixel buffer (Caller must free)
** @param chans    Output: Number of channels (3 for RGB, 4 for CMYK/RGBA)
** @param error    Output: Error message if failed
** @return         GOO_SUCCESS on success, GOO_ERROR_FAILURE on error
*/
int
goo_pdf_png(const char* pdf_path, 
            int page, 
            int x, 
            int y, 
            int w, 
            int h,
            unsigned char** png_data,
            int* chans,
            char** error);

/*!
** Extracts a region from a PDF page and scans it (e.g., for barcodes).
**
** This function orchestrates the process of:
** 1. Rendering a specific crop of a PDF page to a raw memory buffer.
** 2. Passing that buffer to the scanning engine.
**
** @param pdf_path Path to the input PDF file
** @param page     Page number (1-based)
** @param x        Crop X origin (in PDF Points)
** @param y        Crop Y origin (in PDF Points)
** @param w        Crop Width (in PDF Points)
** @param h        Crop Height (in PDF Points)
** @param text     Output: Resulting text (Caller must free)
** @param error    Output: Error message if failed
** @return         GOO_SUCCESS on success, GOO_ERROR_FAILURE on error
*/
int
goo_pdf_scan(const char* pdf_path, 
             int page, 
             int x, 
             int y, 
             int w, 
             int h,
             char** text,
             char** error);

#ifdef __cplusplus
}
#endif

#endif // __GOOLIB_PDF_H__ 