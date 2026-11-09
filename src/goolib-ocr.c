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
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <tesseract/baseapi.h>

#include <include/paddleocr.h>
#include <include/paddlestructure.h>
#include <include/utility.h>

using namespace PaddleOCR;

#include "goolib-ocr.h"
#include "goolib-error.h"

static tesseract::TessBaseAPI* tess = NULL;

// init ocr object
// static PaddleOCR::PaddleStructure engine = PaddleOCR::PaddleStructure();
static PPOCR* ppocr = NULL;

static std::string 
goo_ocr_strip(const std::string& s) 
{
  std::string result;
  result.reserve(s.size());
  for (char c : s) {
    if (c != ' ') {
      result.push_back(c);
    }
  }
  return result;
}

static std::string 
goo_ocr_trim(const std::string& s) 
{
  std::string result;
  result.reserve(s.size());
  for (char c : s) {
    if (c != ' ') {
      result.push_back(c);
    }
  }
  return result;
}

static bool 
goo_ocr_cmp(const std::string& a, const std::string& b) 
{
  std::istringstream sa(a);
  std::istringstream sb(b);

  std::string la, lb;

  while (true) {
    // read next non-empty normalized line from a
    bool ra = false;
    while (std::getline(sa, la)) {
      la = goo_ocr_trim(la);
      if (!la.empty()) {
        la = goo_ocr_strip(la);
        ra = true;
        break;
      }
    }

    // read next non-empty normalized line from b
    bool rb = false;
    while (std::getline(sb, lb)) {
      lb = goo_ocr_trim(lb);
      if (!lb.empty()) {
        lb = goo_ocr_strip(lb);
        rb = true;
        break;
      }
    }

    // both reached end
    if (!ra || !rb) {
      return ra == rb;
    }

    // compare normalized lines
    if (la != lb) {
      return false;
    }
  }
}

int
goo_ocr_init(const char* tessdata, const char* langs)
{
  tess = new tesseract::TessBaseAPI();
  ppocr = new PPOCR();
  if (tess->Init(tessdata, langs)) 
  {
    fprintf(stderr, "Could not initialize tesseract.\n");
    delete tess;
    return GOO_ERROR_FAILURE;
  }
  tess->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
  tess->SetVariable("debug_file", "/dev/null");
  // tess->SetPageSegMode(tesseract::PSM_SINGLE_LINE);
  return GOO_SUCCESS;
}

void*
goo_ocr_api(void)
{
  return (void*)tess;
}

int
goo_ocr_text(const unsigned char* img, 
             int width, 
             int height, 
             int channels, 
             char** text)
{
  tess->SetImage(img, width, height, channels, width * channels);  
  *text = tess->GetUTF8Text();
  return GOO_SUCCESS;
}

int
goo_ocr_eq(const unsigned char* img1, 
           int w1, 
           int h1, 
           int c1, 
           const unsigned char* img2, 
           int w2, 
           int h2, 
           int c2)
{
  tess->SetImage(img1, w1, h1, c1, w1 * c1);  
  char* text1 = tess->GetUTF8Text();
  tess->SetImage(img2, w2, h2, c2, w2 * c2);  
  char* text2 = tess->GetUTF8Text();
  const std::string s1(text1);
  const std::string s2(text2);
  if (goo_ocr_cmp(s1, s2)) {
    return GOO_SUCCESS;
  }
  return GOO_ERROR_FAILURE;
}