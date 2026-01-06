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
#ifndef GOOLIB_PNG_H
#define GOOLIB_PNG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <png.h>

/*!
** Reads a PNG file and normalizes it to 32-bit RGBA format.
**
** This function handles various PNG color types (Palette, Gray, RGB)
** and forces them into a standard RGBA layout (4 bytes per pixel).
**
** @param filename Input file path
** @param width    Pointer to store image width
** @param height   Pointer to store image height
** @param channels Pointer to store number of channels
** @return         Pointer to raw RGBA data (width*height*4 bytes), or NULL.
*/
unsigned char*
goo_png_read(const char* filename, int* width, int* height, int* channels);

/*!
** Writes raw pixel data to a PNG file.
**
** This function creates a new PNG file and writes the provided buffer to it.
** It automatically detects the PNG color type (Gray, RGB, or RGBA) based
** on the number of channels provided.
**
** @param filename   Output file path (e.g., "output.png")
** @param image_data Pointer to raw pixel buffer (flat array)
** @param width      Image width in pixels
** @param height     Image height in pixels
** @param channels   Number of color channels (1=Gray, 3=RGB, 4=RGBA)
*/
void
goo_png_write(const char* filename, png_bytep image_data, int width, int height, int channels);

/*! 
** crops a png image.
**
** @param png the input png file.
** @param x the x coordinate of the crop area.
** @param y the y coordinate of the crop area.
** @param w the width of the crop area.
** @param h the height of the crop area.
** @param out the output png file. 
** @param error the error message.
** @return 0 on success, -1 on error.
*/
int 
goo_png_crop(const char* png, 
             int x, 
             int y, 
             int w, 
             int h, 
             const char* out, 
             char** error);

/*!
** Converts RGBA data to 8-bit Grayscale.
**
** This function implements the standard luminance formula:
** Gray = 0.299*R + 0.587*G + 0.114*B
**
** @param rgba_data Input buffer (RGBA format, 4 bytes per pixel)
** @param width     Image width
** @param height    Image height
** @param channels  Number of color channels (1=Gray, 3=RGB, 4=RGBA)
** @return          Pointer to new Grayscale buffer (1 byte per pixel)
*/
unsigned char*
goo_png_grayscale(unsigned char* rgba_data, int width, int height, int channels);

/*!
** Replaces a specific color with another color in an RGBA buffer.
**
** @param data     Pointer to raw RGBA data
** @param width    Image width
** @param height   Image height
** @param channels Number of color channels (1=Gray, 3=RGB, 4=RGBA)
** @param src_r/g/b  The color to find (e.g., 0,0,0 for black)
** @param dst_r/g/b  The color to replace with (e.g., 255,0,0 for red)
*/
void 
goo_png_color(unsigned char* data, 
              int width, int height, int channels,
              int src_r, int src_g, int src_b,
              int dst_r, int dst_g, int dst_b);  

/*!
** Scans a raw image buffer for barcodes or QR codes using ZXing-C++.
**
** This function wraps the raw pixel data into a ZXing ImageView and
** attempts to read a single barcode from it.
**
** @param data     Pointer to raw pixel data
** @param width    Image width
** @param height   Image height
** @param channels Number of channels (1=Gray, 3=RGB, 4=RGBA)
** @param text     Output: Pointer to the result string (Caller must free)
** @return         GOO_SUCCESS if barcode found, GOO_ERROR_FAILURE otherwise
*/
int
goo_png_scan(const unsigned char* data, 
             int width, 
             int height, 
             int channels, 
             char** text);

#ifdef __cplusplus
}
#endif

#endif /* GOOLIB_PNG_H */
