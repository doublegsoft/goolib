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
#include <png.h>
#include <math.h>

#include "goolib-error.h"
#include "goolib-png.h"

unsigned char*
goo_png_read(const char* filename, int* width, int* height) {
  FILE* fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Error: Could not open %s\n", filename);
    return NULL;
  }

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) {
    fclose(fp);
    return NULL;
  }

  png_infop info = png_create_info_struct(png);
  if (!info) {
    png_destroy_read_struct(&png, NULL, NULL);
    fclose(fp);
    return NULL;
  }

  if (setjmp(png_jmpbuf(png))) {
    fprintf(stderr, "Error during PNG read\n");
    png_destroy_read_struct(&png, &info, NULL);
    fclose(fp);
    return NULL;
  }

  png_init_io(png, fp);
  png_read_info(png, info);

  int color_type = png_get_color_type(png, info);
  int bit_depth = png_get_bit_depth(png, info);

  // --- NORMALIZE TO RGBA ---

  // 1. Strip 16-bit to 8-bit
  if (bit_depth == 16)
    png_set_strip_16(png);

  // 2. Expand Paletted images to RGB
  if (color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png);

  // 3. Expand Grayscale < 8-bit to 8-bit
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_expand_gray_1_2_4_to_8(png);

  // 4. Convert Transparency chunks to Alpha channel
  if (png_get_valid(png, info, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png);

  // 5. Convert Grayscale to RGB (so we can process it uniformly later)
  if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png);

  // 6. Add Alpha channel if missing (RGB -> RGBA)
  // This ensures our output is always 4 bytes per pixel
  if (!(color_type & PNG_COLOR_MASK_ALPHA))
    png_set_add_alpha(png, 0xFF, PNG_FILLER_AFTER);

  png_read_update_info(png, info);

  // --- READ DATA ---
  
  *width = png_get_image_width(png, info);
  *height = png_get_image_height(png, info);
  int row_bytes = png_get_rowbytes(png, info);

  // Allocate buffer for RGBA (should be width * height * 4)
  unsigned char* data = (unsigned char*)malloc(row_bytes * (*height));
  if (!data) {
    png_destroy_read_struct(&png, &info, NULL);
    fclose(fp);
    return NULL;
  }

  png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * (*height));
  for (int y = 0; y < *height; y++) {
    row_pointers[y] = data + (y * row_bytes);
  }

  png_read_image(png, row_pointers);

  free(row_pointers);
  png_destroy_read_struct(&png, &info, NULL);
  fclose(fp);

  return data;
}

int 
goo_png_crop(const char* png_path, 
             int x, 
             int y, 
             int w, 
             int h, 
             const char* out_path, 
             char** error)
{
  FILE* fp = fopen(png_path, "rb");
  if (!fp) 
  {  
    *error = goo_error_new("erro to open png file \"%s\"", png_path);
    return GOO_ERROR_FAILURE;
  }

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) 
  { 
    fclose(fp); 
    *error = goo_error_new("png_create_read_struct failed");
    return GOO_ERROR_FAILURE;
  }

  png_infop info = png_create_info_struct(png);
  if (!info) 
  { 
    png_destroy_read_struct(&png, NULL, NULL); 
    fclose(fp); 
    exit(1); 
  }

  if (setjmp(png_jmpbuf(png))) 
  {
    png_destroy_read_struct(&png, &info, NULL);
    fclose(fp);
    *error = goo_error_new("png_create_info_struct failed");
    return GOO_ERROR_FAILURE;
  }

  png_init_io(png, fp);
  png_read_info(png, info);

  int width      = png_get_image_width(png, info);
  int height     = png_get_image_height(png, info);
  png_byte color_type = png_get_color_type(png, info);
  png_byte bit_depth  = png_get_bit_depth(png, info);

  if (bit_depth == 16) png_set_strip_16(png);
  if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png);
  if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);
  if (color_type == PNG_COLOR_TYPE_RGB ||
      color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_PALETTE)
      png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
  if (color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
      png_set_gray_to_rgb(png);

  png_read_update_info(png, info);

  png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
  for (int i = 0; i < height; i++)
    row_pointers[i] = (png_byte*)malloc(png_get_rowbytes(png, info));

  png_read_image(png, row_pointers);
  fclose(fp);

  if (x < 0 || y < 0 || x + w > width || y + h > height) 
  {
    *error = goo_error_new("invalid crop rectangle");
    return GOO_ERROR_FAILURE;
  }

  FILE* out = fopen(out_path, "wb");
  if (!out) 
  { 
    *error = goo_error_new("error to open output file \"%s\"", out_path);
    return GOO_ERROR_FAILURE; 
  }

  png_structp png_w = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info_w = png_create_info_struct(png_w);
  if (setjmp(png_jmpbuf(png_w))) 
  { 
    fclose(out); 
    return GOO_ERROR_FAILURE; 
  }

  png_init_io(png_w, out);

  png_set_IHDR(png_w, info_w, w, h,
               8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png_w, info_w);

  for (int i = 0; i < h; i++) 
  {
    png_bytep row = row_pointers[y + i] + x * 4;
    png_write_row(png_w, row);
  }

  png_write_end(png_w, NULL);

  for (int i = 0; i < height; i++)
    free(row_pointers[i]);
  free(row_pointers);

  png_destroy_read_struct(&png, &info, NULL);
  png_destroy_write_struct(&png_w, &info_w);
  fclose(out);

  return GOO_SUCCESS;
}

unsigned char*
goo_png_grayscale(unsigned char* rgba_data, int width, int height) 
{
  if (!rgba_data) return NULL;

  int pixel_count = width * height;
  unsigned char* gray_data = (unsigned char*)malloc(pixel_count);
  if (!gray_data) return NULL;

  for (int i = 0; i < pixel_count; i++) {
    // RGBA layout: [R, G, B, A]
    // Index calculation: i * 4
    int r = rgba_data[i * 4];
    int g = rgba_data[i * 4 + 1];
    int b = rgba_data[i * 4 + 2];
    
    // Luminance formula (Human perception)
    // Using integer math for speed: (R*30 + G*59 + B*11) / 100
    int gray = (r * 30 + g * 59 + b * 11) / 100;

    gray_data[i] = (unsigned char)gray;
  }

  return gray_data;
}

void
goo_png_write(const char* filename, png_bytep image_data, int width, int height) {
  FILE* fp = fopen(filename, "wb");
  if (!fp) abort();

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) abort();

  png_infop info = png_create_info_struct(png);
  if (!info) abort();

  if (setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);

  png_set_IHDR(
    png,
    info,
    width, height,
    8, 
    PNG_COLOR_TYPE_RGBA,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
  );

  png_write_info(png, info);

  for (int y = 0; y < height; y++) {
    png_bytep row = image_data + (y * width * 4); 
    png_write_row(png, row);
  }

  png_write_end(png, NULL);
  fclose(fp);

  if (png && info)
    png_destroy_write_struct(&png, &info);
}


void 
goo_png_color(unsigned char* data, int width, int height, 
              int src_r, int src_g, int src_b,
              int dst_r, int dst_g, int dst_b) 
{
  if (!data) return;

  int pixel_count = width * height;

  for (int i = 0; i < pixel_count; i++) {
    // Calculate pointer to current pixel (4 bytes per pixel)
    unsigned char* px = &data[i * 4];

    // Get current values
    int r = px[0];
    int g = px[1];
    int b = px[2];
    int a = px[3];

    // Check if it matches the Source Color (Black)
    // We also check 'a > 0' to ensure we don't color transparent pixels
    if (r == src_r && g == src_g && b == src_b && a > 0) {
      
      // Apply new color
      px[0] = (unsigned char)dst_r;
      px[1] = (unsigned char)dst_g;
      px[2] = (unsigned char)dst_b;
      
      // Note: We leave Alpha (px[3]) alone, maintaining original opacity
    }
  }
}
