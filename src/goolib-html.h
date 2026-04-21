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
#ifndef GOOLIB_HTML_H
#define GOOLIB_HTML_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/*!
** @brief Convert HTML to Markdown according to ChatGPT html format
**
** @param html The HTML string to convert
** @param md Pointer to store the resulting Markdown string
** @param md_len Pointer to store the length of the Markdown string
*/
void
goo_html_chatgpt(const char* html, char** md, size_t* md_len);

/*!
** @brief Convert HTML to Markdown according to Gemini html format
**
** @param html The HTML string to convert
** @param md Pointer to store the resulting Markdown string
** @param md_len Pointer to store the length of the Markdown string
*/
void
goo_html_gemini(const char* html, char** md, size_t* md_len);

/*!
** @brief Convert HTML to Markdown according to Grok html format
**
** @param html The HTML string to convert
** @param md Pointer to store the resulting Markdown string
** @param md_len Pointer to store the length of the Markdown string
*/
void
goo_html_grok(const char* html, char** md, size_t* md_len);

/*!
** @brief Convert HTML to Markdown according to Claude html format
**
** @param html The HTML string to convert
** @param md Pointer to store the resulting Markdown string
** @param md_len Pointer to store the length of the Markdown string
*/
void
goo_html_claude(const char* html, char** md, size_t* md_len);

#ifdef __cplusplus
}
#endif

#endif // GOOLIB_HTML_H