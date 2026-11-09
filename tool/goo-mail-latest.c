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

/*!
** IMAP 收件箱回调函数，用于从响应中接收 UID SEARCH UNSEEN 的返回结果。
*/
static size_t 
goo_mail_unseen_cb(void* contents, size_t size, size_t nmemb, void* userp) 
{
  size_t total = size * nmemb;
  char* buf = (char*) userp;
  // 拼接返回结果（格式通常为: "* SEARCH 3 8 15\r\n"）
  strncat(buf, (char*) contents, total);
  return total;
}

/*!
** IMAP 收件箱回调函数，用于从服务器响应数据中提取 "UIDNEXT" 的值。
*/
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

/*!
** 邮件数据流回调函数，用于将接收到的邮件数据块直接写入文件。
*/
static size_t 
goo_mail_latest_cb(void* contents, size_t size, size_t nmemb, void* userp) 
{
  FILE* fp = (FILE*) userp;
  fwrite(contents, size, nmemb, fp);
  fflush(fp);
  return size * nmemb;
}

/*!
** 从原始邮件头信息字符串中，提取指定头部字段的值（如 "Subject:", "From:"）。
*/
static char* 
goo_mail_header_get(char* str, const char* header)
{
  char* found = strstr(str, header);
  if (found == NULL)
    return NULL;
    
  found += strlen(header);
  
  int len = 0;
  char* scan = found;
  while (*scan != '\n' && *scan != '\r' && *scan != '"') 
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
    if (custom_request != NULL)
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, custom_request);

    if (cb != NULL)
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    if (userdata != NULL) 
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, userdata); 

    res = curl_easy_perform(curl);

    if(res != CURLE_OK)
    {
      if (error != NULL)
        *error = strdup(curl_easy_strerror(res));
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }
 
    curl_easy_cleanup(curl);
  }
 
  curl_global_cleanup();
  return (res == CURLE_OK) ? GOO_SUCCESS : GOO_ERROR_FAILURE;
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
  char inbox_url[4096] = {'\0'};
  snprintf(inbox_url, sizeof(inbox_url), "imaps://%s/INBOX", mail_imap_host);

  // 1. 查找所有未读邮件的 UID
  char search_buf[4096] = {'\0'};
  int status = goo_mail_request(inbox_url, 
                                mail_imap_port,
                                mail_imap_user,
                                mail_imap_password,
                                "UID SEARCH UNSEEN",
                                goo_mail_unseen_cb,
                                search_buf,
                                error);
  if (status != GOO_SUCCESS)
  {
    return status;
  }

  // 2. 解析出列表中最后一个 UID（即最新未读邮件）
  // 返回的内容通常类似 "* SEARCH 10 12 18\r\n"，如果没有未读则是 "* SEARCH\r\n"
  long latest_uid = -1;
  char* token = strtok(search_buf, " \r\n");
  while (token != NULL)
  {
    long val = strtol(token, NULL, 10);
    if (val > 0)
    {
      latest_uid = val;
    }
    token = strtok(NULL, " \r\n");
  }

  // 如果没有发现任何未读邮件，直接返回
  if (latest_uid <= 0)
  {
    if (error != NULL)
      *error = strdup("No unseen mail found");
    return GOO_ERROR_FAILURE;
  }

  // 3. 构造下载最新未读邮件的 URL
  char url[4096] = {'\0'};
  snprintf(url, sizeof(url), "imaps://%s/INBOX;uid=%ld", mail_imap_host, latest_uid);
  
  char tmpmail[8192] = {'\0'};
  snprintf(tmpmail, sizeof(tmpmail), "%s/mail.txt", temp_dir);

  char tmpatt[8192] = {'\0'};
  snprintf(tmpatt, sizeof(tmpatt), "%s/att.txt", temp_dir);
  
  FILE* fp = fopen(tmpmail, "w");
  if (fp == NULL)
  {
    if (error != NULL)
      *error = strdup("failed to open file for writing mail");
    return GOO_ERROR_FAILURE;
  }

  status = goo_mail_request(url, 
                            mail_imap_port,
                            mail_imap_user,
                            mail_imap_password,
                            NULL,
                            goo_mail_latest_cb,
                            (void*)fp,
                            error);
  fclose(fp);

  if (status != GOO_SUCCESS)
  {
    remove(tmpmail);
    return status;
  }

  // 4. 将该邮件在服务端标记为已读 (\Seen)
  char store_cmd[128] = {'\0'};
  snprintf(store_cmd, sizeof(store_cmd), "UID STORE %ld +FLAGS (\\Seen)", latest_uid);
  goo_mail_request(inbox_url,
                   mail_imap_port,
                   mail_imap_user,
                   mail_imap_password,
                   store_cmd,
                   NULL,
                   NULL,
                   NULL);

  // 5. 提取并解码邮件附件
  FILE* fpb64 = fopen(tmpatt, "w");
  if (fpb64 == NULL)
  {
    if (error != NULL)
      *error = strdup("failed to open file for writing attachment");
    remove(tmpmail);
    return GOO_ERROR_FAILURE;
  }

  fp = fopen(tmpmail, "r");
  if (fp == NULL)
  {
    if (error != NULL)
      *error = strdup("failed to open mail file for parsing");
    fclose(fpb64);
    remove(tmpmail);
    remove(tmpatt);
    return GOO_ERROR_FAILURE;
  }

  char attname[8192] = {'\0'};
  char line[1024];
  int found_attachment = 0;
  int start_attachment = 0;

  while (fgets(line, sizeof(line), fp) != NULL) 
  {
    if (strstr(line, "Content-Type: application") != NULL)
    {
      found_attachment = 1;
      continue;
    }
    if (found_attachment == 1)
    {
      const char* attstr = "Content-Disposition: attachment; filename=";
      if (strstr(line, attstr) != NULL)
      {
        char* scan = line + strlen(attstr);
        if (*scan == '"') scan++;
        int index = 0;
        while (*scan != '\r' && *scan != '\n' && *scan != '"')
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

  // 6. Base64 解码附件并输出到 download_dir
  if (strlen(attname) > 0)
  {
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* src = BIO_new_file(tmpatt, "rb");
    BIO* chain = BIO_push(b64, src);     

    char dlpath[8192] = {'\0'};
    snprintf(dlpath, sizeof(dlpath), "%s/%s", download_dir, attname);

    unsigned char buf[4096];
    int n;
    fp = fopen(dlpath, "wb");
    if (fp != NULL)
    {
      while ((n = BIO_read(chain, buf, sizeof(buf))) > 0) 
      {
        size_t written = fwrite(buf, 1, (size_t)n, fp);
        if (written != (size_t)n) 
        {
          BIO_free_all(chain);
          fclose(fp);
          remove(tmpmail);
          remove(tmpatt);
          return GOO_ERROR_FAILURE;
        }
      } 
      fclose(fp);
    }
    BIO_free_all(chain);
  }

  // 7. 清理临时缓存文件
  remove(tmpmail);
  remove(tmpatt);

  return GOO_SUCCESS;
}