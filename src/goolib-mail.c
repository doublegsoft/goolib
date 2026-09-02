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
** IMAP 收件箱回调函数，用于从服务器响应数据中提取 "UIDNEXT" 的值。
**
** @param contents 指向接收到的数据缓冲区的指针。
** @param size 每个数据元素的大小。
** @param nmemb 数据元素的数量。
** @param userp 用户自定义指针，此处用于接收输出的 UID 字符串缓冲区。
** @return 实际处理的字节数。
*/
static size_t 
goo_mail_inbox_cb(void* contents, size_t size, size_t nmemb, void* userp) 
{
  char* uid = (char*) userp;
  char* str = (char*) contents;
  
  // 在响应报文中查找 "UIDNEXT" 关键字的位置
  char* uidnext = strstr(str, "UIDNEXT");
  if (uidnext != NULL)
  {
    // 跳过 "UIDNEXT " 的长度，将指针移动到具体数值的起始位置
    char* val = uidnext + strlen("UIDNEXT ");
    int index = 0;
    
    // 循环拷贝字符，直到遇到右中括号 ']' 结束
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
**
** @param contents 指向接收到的数据缓冲区的指针。
** @param size 每个数据元素的大小。
** @param nmemb 数据元素的数量.
** @param userp 用户自定义指针，此处为指向目标 FILE 结构体的指针。
** @return 写入文件的实际数据字节数。
*/
static size_t 
goo_mail_latest_cb(void* contents, size_t size, size_t nmemb, void* userp) 
{
  FILE* fp = (FILE*) userp;
  
  // 将当前数据分片直接写入文件，并刷新缓冲区
  fwrite(contents, size, nmemb, fp);
  fflush(fp);

  // 备用方案（已注释）：在内存中动态分配并拼接完整的邮件内容字符串
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

/*!
** 从原始邮件头信息字符串中，提取指定头部字段的值（如 "Subject:", "From:"）。
**
** @param str 包含完整邮件头部信息的字符串。
** @param header 需要检索的头部字段名称（需包含冒号和空格，例如 "Subject: "）。
** @return 动态分配的包含头部字段值的字符串。未找到时返回 NULL。
**         注意：调用者需负责 free 释放返回的内存。
*/
static char* 
goo_mail_header_get(char* str, const char* header)
{
  // 定位目标头部字段在头部信息中的起始位置
  char* found = strstr(str, header);
  if (found == NULL)
    return NULL;
    
  // 将指针向后移动，跳过字段名称本身，指向字段值的起点
  found += strlen(header);
  
  // 计算该头部字段值的实际长度（直到遇到换行符 \n 或 \r）
  int len = 0;
  char* scan = found;
  while (*scan != '\n' && *scan != '\r') 
  {
    scan++;
    len++;
  }
  
  // 为提取的值分配内存（包含额外的空字符终止符 \0）
  char* ret = (char*) malloc(len + 1);
  int index = 0;
  
  // 将该字段的值拷贝到新分配的内存空间中
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