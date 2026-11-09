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
#ifndef __GOOLIB_OCR_H__
#define __GOOLIB_OCR_H__  

#ifdef __cplusplus
extern "C" {
#endif

int
goo_ocr_init(const char* tessdata, const char* langs);

void*
goo_ocr_api(void);

int
goo_ocr_text(const unsigned char* data, 
             int width, 
             int height, 
             int channels, 
             char** text);

/*!
** Compare two images by OCR result.
**
** This function performs OCR on two input images using Tesseract,
** extracts UTF-8 text from each image, and compares the recognized
** text content using goo_ocr_cmp().
**
** @param img1 Pointer to image1 raw pixel buffer
** @param w1   Width of image1
** @param h1   Height of image1
** @param c1   Number of channels of image1 (e.g. 1=gray, 3=RGB)
** @param img2 Pointer to image2 raw pixel buffer
** @param w2   Width of image2
** @param h2   Height of image2
** @param c2   Number of channels of image2
**
** @return GOO_SUCCESS if OCR texts are considered equal,
**         GOO_ERROR_FAILURE otherwise
*/
int
goo_ocr_eq(const unsigned char* img1, 
           int w1, 
           int h1, 
           int c1, 
           const unsigned char* img2, 
           int w2, 
           int h2, 
           int c2);

#ifdef __cplusplus
}
#endif

#endif // __GOOLIB_OCR_H__
