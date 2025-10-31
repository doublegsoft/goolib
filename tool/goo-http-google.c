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
 
#include <curl/curl.h>
 
int main(void)
{
  CURL *curl;
  CURLcode res;
  FILE* headerfile;
  const char* pPassphrase = NULL;
 
  static const char* pCertFile = "testcert.pem";
  static const char* pCACertFile = "cacert.pem";
  static const char* pHeaderFile = "dumpit";
 
  const char *pKeyName;
  const char *pKeyType;
 
  const char *pEngine;
 
#ifdef USE_ENGINE
  pKeyName  = "rsa_test";
  pKeyType  = "ENG";
  pEngine   = "chil";            /* for nCipher HSM... */
#else
  pKeyName  = "testkey.pem";
  pKeyType  = "PEM";
  pEngine   = NULL;
#endif
 
  headerfile = fopen(pHeaderFile, "wb");
  if(!headerfile)
    return 1;
 
  res = curl_global_init(CURL_GLOBAL_ALL);
  if(res) {
    fclose(headerfile);
    return (int)res;
  }
 
  curl = curl_easy_init();
  if(curl) {
    /* what call to write: */
    curl_easy_setopt(curl, CURLOPT_URL, "https://www.google.com");
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, headerfile);
 
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4127)  /* conditional expression is constant */
#endif
    do { /* dummy loop, just to break out from */
      if(pEngine) {
        /* use crypto engine */
        if(curl_easy_setopt(curl, CURLOPT_SSLENGINE, pEngine) != CURLE_OK) {
          /* load the crypto engine */
          fprintf(stderr, "cannot set crypto engine\n");
          break;
        }
        if(curl_easy_setopt(curl, CURLOPT_SSLENGINE_DEFAULT, 1L) != CURLE_OK) {
          /* set the crypto engine as default */
          /* only needed for the first time you load
             an engine in a curl object... */
          fprintf(stderr, "cannot set crypto engine as default\n");
          break;
        }
      }

      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

      res = curl_easy_perform(curl);
      /* Check for errors */
      if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
 
      /* we are done... */
    } while(0);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
    /* always cleanup */
    curl_easy_cleanup(curl);
  }
 
  curl_global_cleanup();
 
  fclose(headerfile);
 
  return 0;
}