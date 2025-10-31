#include <stdio.h>
 
#include <curl/curl.h>

 
int main(void)
{
  CURL* curl;
  CURLcode res;

  res = curl_global_init(CURL_GLOBAL_ALL);
  if(res) {
    return (int)res;
  }
 
  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:11434/api/generate");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{"
      "\"model\": \"gpt-oss:20b\","
      "\"prompt\": \"cjson assebmle a json object exmaple\","
      "\"stream\": false"
    "}");
 
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4127) 
#endif
    do { 

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

    curl_easy_cleanup(curl);
  }
 
  curl_global_cleanup();
 
  return 0;
}