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
#include <GlobalParams.h>
#include <PDFDoc.h>
#include <SplashOutputDev.h>
#include <splash/SplashBitmap.h>
#include <splash/SplashTypes.h>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <png.h>

#include "goolib-pdf.h"
#include "goolib-png.h"
#include "goolib-error.h"

/*!
** Converts a SplashBitmap to a standard packed pixel buffer.
**
** This removes internal padding/stride from the SplashBitmap so the
** result is a continuous array suitable for fwrite, libpng, etc.
**
** @param bitmap   Source SplashBitmap
** @param width    Output: stores width
** @param height   Output: stores height
** @param channels Output: stores bytes per pixel (1 or 3)
** @return         New allocated buffer (Caller must free)
*/
static unsigned char* 
goo_bitmap_to_buffer(SplashBitmap* bitmap, 
                     int* width, 
                     int* height, 
                     int* channels) 
{
  if (!bitmap) return NULL;

  *width = bitmap->getWidth();
  *height = bitmap->getHeight();

  // 1. Determine Channels based on Splash Mode
  SplashColorMode mode = bitmap->getMode();
  
  switch (mode) {
    case splashModeMono8:
      *channels = 1;
      break;
    case splashModeRGB8:
    case splashModeBGR8:
      *channels = 3;
      break;
    // case splashModeCMYK8: // Rare in standard PDF to Image
    //   *channels = 4;
    //   break;
    default:
      fprintf(stderr, "Unsupported Splash Mode\n");
      return NULL;
  }

  // 2. Allocate new packed buffer
  // Size = W * H * Channels
  long buffer_size = (*width) * (*height) * (*channels);
  unsigned char* buffer = (unsigned char*)malloc(buffer_size);
  if (!buffer) return NULL;

  // 3. Copy Row by Row (Handling Stride)
  // SplashBitmap->getDataPtr() returns the raw internal buffer
  unsigned char* src_data = bitmap->getDataPtr();
  int src_row_stride = bitmap->getRowSize(); // Includes padding
  
  int dest_row_stride = (*width) * (*channels); // Tightly packed

  for (int y = 0; y < *height; y++) {
    // Source: Start of row Y (with padding)
    unsigned char* src_row = src_data + (y * src_row_stride);
    
    // Dest: Start of row Y (tightly packed)
    unsigned char* dest_row = buffer + (y * dest_row_stride);

    // Copy only the valid pixels, ignore the padding at the end
    memcpy(dest_row, src_row, dest_row_stride);
  }

  return buffer;
}

int
goo_pdf_text(const char* pdf_path, 
             int page, 
             int x, 
             int y, 
             int w, 
             int h,
             char** text,
             char** error)
{
  if (!globalParams) {
    globalParams = new GlobalParams(NULL);
    globalParams->setErrQuiet(gTrue);
  }
  tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
  const char* tessdata = "/Users/christian/export/local/models/tessdata"; 
  if (api->Init(tessdata, "eng")) 
  {
    fprintf(stderr, "Could not initialize tesseract.\n");
    delete api;
    return GOO_ERROR_FAILURE;
  }
  api->SetPageSegMode(tesseract::PSM_SINGLE_LINE);

  GString* fname = new GString(pdf_path);
  PDFDoc* doc = new PDFDoc(fname); 
  if (!doc->isOk()) 
  {
    delete doc;
    *error = goo_error_new("could not open pdf \"%s\"", pdf_path);
    return GOO_ERROR_FAILURE;
  }

  SplashColor paperColor;
  paperColor[0] = 0xff; 
  paperColor[1] = 0xff; 
  paperColor[2] = 0xff;

  // splashModeRGB8 = 24-bit RGB
  // 4 = sub-pixel anti-aliasing off (standard)
  // false = bitmap upside down? (false = top-to-bottom)
  SplashOutputDev* splash = new SplashOutputDev(splashModeRGB8, 3, false, paperColor);
  
  // Connect device to document
  splash->startDoc(doc->getXRef());

  // 4. Render the Specific Slice
  // Note on coordinates: PDF usually uses Bottom-Left as (0,0).
  // displayPageSlice usually handles coordinate transformation logic
  // but generally expects Top-Left logic for the slice box.
  doc->displayPageSlice(
    splash,       // Output device
    page,         // Page number
    300, 300,     // Horizontal/Vertical DPI
    0,            // Rotation
    false,        // Use MediaBox?
    true,         // Crop?
    false,        // Printing?
    x, y, w, h    // The slice box (x, y, w, h)
  );

  SplashBitmap* bitmap = splash->getBitmap();

  int channels;
  unsigned char* raw_data = goo_bitmap_to_buffer(bitmap, &w, &h, &channels);

  if (raw_data) 
  {
    // Note: channels will be 3 for RGB8, 1 for Mono8
    // stride is w * channels because we packed it tightly
    api->SetImage(raw_data, w, h, channels, w * channels);
    
    *text = api->GetUTF8Text();
    free(raw_data);
  }

  api->End();
  delete api;
  delete splash;
  delete doc;

  return GOO_SUCCESS;
}

int
goo_pdf_png(const char* pdf_path, 
             int page, 
             int x, 
             int y, 
             int w, 
             int h,
             unsigned char** png_data,
             int* chans,
             char** error)
{
  if (!globalParams) {
    globalParams = new GlobalParams(NULL);
    globalParams->setErrQuiet(gTrue);
  }

  GString* fname = new GString(pdf_path);
  PDFDoc* doc = new PDFDoc(fname); // doc owns fname

  if (!doc->isOk()) {
    delete doc;
    return -1;
  }

  x = x < 0 ? 0 : x;
  y = y < 0 ? 0 : y;

  Page* p = doc->getCatalog()->getPage(page);
  
  if (!p) {
    delete doc;
    return GOO_ERROR_FAILURE;
  } 

  if (w < 0) 
  {
    w = p->getCropWidth();
    w = ((w * 300 / 72.0) + 0.5);
  }
  if (h < 0)
  {
    h = p->getCropHeight();
    h = ((h * 300 / 72.0) + 0.5);
  }

  // Use array to prevent segfaults on some Poppler versions
  SplashColor paperColor;
  paperColor[0] = 0xff; 
  paperColor[1] = 0xff; 
  paperColor[2] = 0xff;

  SplashOutputDev* splash = new SplashOutputDev(splashModeRGB8, 3, false, paperColor);
  splash->startDoc(doc->getXRef());

  // displayPageSlice maps the PDF coordinates to the Bitmap pixels
  doc->displayPageSlice(
    splash,       // Device
    page,         // Page Number
    300, 300,     // Resolution
    0,            // Rotate
    false,        // useMediaBox
    true,         // crop
    false,        // printing
    (int)x, (int)y, (int)w, (int)h // The Crop Rect
  );

  SplashBitmap* bitmap = splash->getBitmap();

  *png_data = goo_bitmap_to_buffer(bitmap, &w, &h, chans);

  delete splash;
  delete doc;

  return GOO_SUCCESS;
}

int
goo_pdf_scan(const char* pdf_path, 
             int page, 
             int x, 
             int y, 
             int w, 
             int h,
             char** text,
             char** error)
{
  if (!globalParams) {
    globalParams = new GlobalParams(NULL);
    globalParams->setErrQuiet(gTrue);
  }

  int ret;
  int chans = 0;
  unsigned char* png_data = NULL;

  ret = goo_pdf_png(pdf_path, page, x, y, w, h, &png_data, &chans, error);
  
  if (ret != GOO_SUCCESS) 
  {
    fprintf(stderr, "%s\n", *error);
    return ret;
  }
  return goo_png_scan(png_data, w, h, chans, text);
}