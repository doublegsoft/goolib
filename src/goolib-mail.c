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
#define GOOLIB_MAIL_H

#include <stdlib.h>
#include <libgen.h>
#include <string.h>

#include <curl/curl.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

#include "internal/smtp.h"
#include "goolib-error.h"
#include "goolib-util.h"
#include "goolib-mail.h"

static size_t 
goo_mail_inbox_cb(void* contents, size_t size, size_t nmemb, void* userp) 
{
  char* uid = (char*) userp;
  char* str = (char*) contents;
  char* uidnext = strstr(str, "UIDNEXT");
  if (uidnext != NULL)
  {
    char* val = uidnext + strlen("UIDNEXT ");
    int index = 0;
    while (*val != ']') 
    {
      uid[index++] = *val;
      val++;
    }
    uid[index] = '\0';
  }
  return size * nmemb;
}

static size_t 
goo_mail_latest_cb(void* contents, size_t size, size_t nmemb, void* userp) 
{
  FILE* fp = (FILE*) userp;
  fwrite(contents, size, nmemb, fp);
  fflush(fp);

  // char** str = (char**) userp;
  // if (*str == NULL) 
  // {
  //   *str = (char*) malloc(size * nmemb + 1);
  //   memcpy(*str, contents, size * nmemb);
  //   (*str)[size * nmemb] = '\0';
  // }
  // else 
  // {
  //   *str = (char*) realloc(*str, strlen(*str) + size * nmemb + 1);
  //   memcpy(*str + strlen(*str), contents, size * nmemb);
  //   (*str)[strlen(*str) + size * nmemb] = '\0';
  // }
  return size * nmemb;
}

static char* 
goo_mail_header_get(char* str, const char* header)
{
  char* found = strstr(str, header);
  if (found == NULL)
    return NULL;
  found += strlen(header);
  int len = 0;
  char* scan = found;
  while (*scan != '\n' && *scan != '\r') 
  {
    scan++;
    len++;
  }
  char* ret = (char*) malloc(len + 1);
  int index = 0;
  while (*found != '\n' && *found != '\r')
  {
    ret[index++] = *found;
    found++;
  }
  ret[index] = '\0';
  return ret;
}

int 
goo_mail_request(const char* mail_imap_url,
                 const char* mail_imap_port,
                 const char* mail_imap_user,
                 const char* mail_imap_password,
                 const char* custom_request,
                 size_t      (*cb)(void*, size_t, size_t, void*),
                 void*       userdata,
                 char**      error)
{
  CURL *curl;
 
  CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
  if(res)
    return (int)res;
 
  curl = curl_easy_init();
  if (curl) 
  {
    curl_easy_setopt(curl, CURLOPT_USERNAME, mail_imap_user);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, mail_imap_password);
    curl_easy_setopt(curl, CURLOPT_URL, mail_imap_url);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, custom_request);

    if (cb != NULL)
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    if (userdata != NULL) 
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, userdata); 

    res = curl_easy_perform(curl);

    if(res != CURLE_OK)
      fprintf(stderr, 
              "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 
    curl_easy_cleanup(curl);
  }
 
  curl_global_cleanup();
  return GOO_SUCCESS;
}

int 
goo_mail_send(const char* mail_smtp_host, 
              const char* mail_smtp_port, 
              const char* mail_smtp_user, 
              const char* mail_smtp_password, 
              const char* from, 
              const char* to, 
              const char* subject, 
              const char* body,
              const char* attachment,
              char**      error)
{
  struct smtp *smtp;
  int rc;
  rc = smtp_open(mail_smtp_host,
                 mail_smtp_port,
                 SMTP_SECURITY_TLS,
                 SMTP_NO_CERT_VERIFY,
                 NULL,
                 &smtp);
  rc = smtp_auth(smtp,
                 SMTP_AUTH_PLAIN,
                 mail_smtp_user,
                 mail_smtp_password);
  rc = smtp_address_add(smtp,
                        SMTP_ADDRESS_FROM,
                        from,
                        from);
  rc = smtp_address_add(smtp,
                        SMTP_ADDRESS_TO,
                        to,
                        to);
  rc = smtp_header_add(smtp,
                       "Subject",
                       subject);
  
  if (attachment != NULL) 
  {
    char* copy = strdup(attachment);
    char* name = basename(copy);             
    smtp_attachment_add_path(smtp, name, attachment);
    free(copy); 
  }
  rc = smtp_mail(smtp, body);
  if(rc != SMTP_STATUS_OK)
  {
    *error = strdup(smtp_status_code_errstr(rc));
    return GOO_ERROR_FAILURE;
  }
  smtp_close(smtp);
  return GOO_SUCCESS;
}

int 
goo_mail_latest(const char* mail_imap_host,
                const char* mail_imap_port,
                const char* mail_imap_user,
                const char* mail_imap_password,
                const char* temp_dir,
                const char* download_dir,
                char**      error)
{
  char url[4096] = {'\0'};
  strcpy(url, "imaps://");
  strcat(url, mail_imap_host);
  strcat(url, "/INBOX");

  char* uidnext = (char*) malloc(128);
  goo_mail_request(url, 
                   mail_imap_port,
                   mail_imap_user,
                   mail_imap_password,
                   "SELECT INBOX",
                   goo_mail_inbox_cb,
                   uidnext,
                   error);
  
  long uid = strtol(uidnext, NULL, 10);
  uid -= 1;
  free(uidnext);
  
  char uidstr[128] = {'\0'};
  sprintf(uidstr, "%ld", uid);

  strcpy(url, "imaps://");
  strcat(url, mail_imap_host);
  strcat(url, "/INBOX;uid=");
  strcat(url, uidstr);
  
  char tmpmail[8192] = {'\0'};
  strcpy(tmpmail, temp_dir);
  strcat(tmpmail, "/mail.txt");

  char tmpatt[8192] = {'\0'};
  strcpy(tmpatt, temp_dir);
  strcat(tmpatt, "/att.txt");
  
  FILE* fp = fopen(tmpmail, "w");
  if (fp == NULL)
  {
    *error = strdup("failed to open file");
    return GOO_ERROR_FAILURE;
  }
  goo_mail_request(url, 
                   mail_imap_port,
                   mail_imap_user,
                   mail_imap_password,
                   NULL,
                   goo_mail_latest_cb,
                   (void*)fp,
                   error);

  fclose(fp);                 
  
  FILE* fpb64 = fopen(tmpatt, "w");
  if (fpb64 == NULL)
  {
    *error = strdup("failed to open file");
    return GOO_ERROR_FAILURE;
  }
  fp = fopen(tmpmail, "r");

  char attname[8192] = {'\0'};
  char* prevline = NULL;
  char line[1024];
  int found_attachment = 0;
  int start_attachment = 0;

  while (fgets(line, 1024, fp) != NULL) 
  {
    if (strstr(line, "Content-Type: application") != NULL)
    {
      found_attachment = 1;
      continue;
    }
    if (found_attachment == 1)
    {
      const char* attstr = "Content-Disposition: attachment; filename=";
      if (strstr(line, "Content-Disposition: attachment; filename=") != NULL)
      {
        char* scan = line + strlen(attstr);
        int index = 0;
        while (*scan != '\r' && *scan != '\n')
        {
          attname[index++] = *scan;
          scan++;
        }
        attname[index] = '\0';
      }
      if (line[0] == '\r' && line[1] == '\n')
      {
        start_attachment = 1;
        continue;
      }
    }
    if (start_attachment == 1)
    {
      
      if (strstr(line, "------") == line)
      {
        start_attachment = 0;
        found_attachment = 0;
        continue;
      }
      fwrite(line, 1, strlen(line), fpb64);
      fflush(fpb64);
    }
  }
  fclose(fp);
  fclose(fpb64);

  BIO* b64 = BIO_new(BIO_f_base64());
  BIO* src = BIO_new_file(tmpatt, "rb");
  BIO* chain = BIO_push(b64, src);     

  char dlpath[8192] = {'\0'};
  strcpy(dlpath, download_dir);
  strcat(dlpath, "/");
  strcat(dlpath, attname);

  unsigned char buf[4096];
  int n;
  fp = fopen(dlpath, "wb");
  while ((n = BIO_read(chain, buf, sizeof(buf))) > 0) 
  {
    size_t written = fwrite(buf, 1, (size_t)n, fp);
    if (written != (size_t)n) 
    {
      perror("fwrite");
      BIO_free_all(chain);
      fclose(fp);
      return GOO_ERROR_FAILURE;
    }
  } 
  BIO_free_all(chain);
  fclose(fp);

  remove(tmpmail);
  remove(tmpatt);

  return GOO_SUCCESS;
}