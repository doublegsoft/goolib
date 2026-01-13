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
#include <tesseract/baseapi.h>

#include "goolib-ocr.h"
#include "goolib-error.h"

static tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();

int
goo_ocr_init(const char* tessdata, const char* langs)
{
  if (api->Init(tessdata, langs)) 
  {
    fprintf(stderr, "Could not initialize tesseract.\n");
    delete api;
    return GOO_ERROR_FAILURE;
  }
  api->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
  // api->SetPageSegMode(tesseract::PSM_SINGLE_LINE);
  return GOO_SUCCESS;
}

void*
goo_ocr_api(void)
{
  return (void*)api;
}

int
goo_ocr_text(const unsigned char* img, 
             int width, 
             int height, 
             int channels, 
             char** text)
{
  api->SetImage(img, width, height, channels, width * channels);  
  *text = api->GetUTF8Text();
  return GOO_SUCCESS;
}