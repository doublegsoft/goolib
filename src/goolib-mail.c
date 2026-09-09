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

#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include <ctype.h>

#include <curl/curl.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

#include "internal/smtp.h"
#include "goolib-error.h"
#include "goolib-util.h"
#include "goolib-mail.h"

/*!
** 解码 RFC 2047 MIME 编码的文件名（例如将 =?UTF-8?B?... 还原为真实的中文原文件名）
*/
static void 
decode_mime_filename(char* dest, const char* src)
{
  if (src == NULL || *src == '\0')
  {
    dest[0] = '\0';
    return;
  }

  // 检查是否包含 UTF-8 Base64 编码标识
  const char* p_start = strstr(src, "=?UTF-8?B?");
  if (p_start == NULL)
    p_start = strstr(src, "=?utf-8?B?");
  if (p_start == NULL)
    p_start = strstr(src, "=?UTF-8?b?");

  if (p_start != NULL)
  {
    p_start += 10; // 跳过 "=?UTF-8?B?"
    const char* p_end = strstr(p_start, "?=");
    if (p_end != NULL)
    {
      char b64_str[2048] = {'\0'};
      size_t b64_len = p_end - p_start;
      if (b64_len < sizeof(b64_str))
      {
        strncpy(b64_str, p_start, b64_len);

        BIO* b64 = BIO_new(BIO_f_base64());
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
        BIO* mem = BIO_new_mem_buf(b64_str, (int)b64_len);
        BIO* bio = BIO_push(b64, mem);

        int decoded_len = BIO_read(bio, dest, 4096);
        if (decoded_len > 0)
        {
          dest[decoded_len] = '\0';
          BIO_free_all(bio);
          return;
        }
        BIO_free_all(bio);
      }
    }
  }

  // 如果不是 MIME 编码格式，直接保留原文件名
  strcpy(dest, src);
}

/*!
** 用于接收 "UID SEARCH UNSEEN" 服务器响应数据的回调函数
*/
static size_t 
goo_mail_search_cb(void* contents, size_t size, size_t nmemb, void* userp) 
{
  size_t total = size * nmemb;
  char* buf = (char*) userp;
  strncat(buf, (char*) contents, total);
  return total;
}

/*!
** 邮件数据流回调函数，用于将接收到的邮件数据块直接写入文件。
*/
static size_t 
goo_mail_download(void* contents, size_t size, size_t nmemb, void* userp) 
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

/*!
** 从下载的 raw 邮件文件 (mail.txt) 中提取纯文本正文内容
**
** @param mail_file 邮件临时文件路径 (如 /tmp/mail.txt)
** @return 动态分配的正文字符串指针（调用方需负责 free() 释放）
*/
static char* 
goo_mail_body(const char* mail_file)
{
  FILE* fp = fopen(mail_file, "r");
  if (fp == NULL)
    return NULL;

  char line[1024];
  int in_text_part = 0;
  int is_base64 = 0;
  int start_reading = 0;

  // 动态扩容的正文缓冲区
  size_t body_cap = 4096;
  size_t body_len = 0;
  char* body_buf = (char*) malloc(body_cap);
  body_buf[0] = '\0';

  while (fgets(line, sizeof(line), fp) != NULL)
  {
    // 1. 寻找纯文本区域
    if (strstr(line, "Content-Type: text/plain") != NULL)
    {
      in_text_part = 1;
      continue;
    }

    // 2. 检查正文的传输编码是否为 Base64
    if (in_text_part && !start_reading)
    {
      if (strstr(line, "Content-Transfer-Encoding: base64") != NULL ||
          strstr(line, "content-transfer-encoding: base64") != NULL)
      {
        is_base64 = 1;
      }
    }

    // 3. 遇到空行（\r\n），表示正文头部结束，正文内容开始
    if (in_text_part && !start_reading && (line[0] == '\r' || line[0] == '\n'))
    {
      start_reading = 1;
      continue;
    }

    // 4. 读取正文数据
    if (start_reading)
    {
      // 遇到 MIME 分割线（--xxxx），说明正文结束
      if (strncmp(line, "--", 2) == 0)
      {
        break;
      }

      size_t line_len = strlen(line);
      // 动态扩容
      if (body_len + line_len + 1 >= body_cap)
      {
        body_cap *= 2;
        body_buf = (char*) realloc(body_buf, body_cap);
      }

      memcpy(body_buf + body_len, line, line_len);
      body_len += line_len;
      body_buf[body_len] = '\0';
    }
  }
  fclose(fp);

  if (body_len == 0)
  {
    free(body_buf);
    return NULL;
  }

  // 5. 如果正文被 Base64 编码过，进行解码
  if (is_base64)
  {
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* mem = BIO_new_mem_buf(body_buf, (int)body_len);
    BIO* bio = BIO_push(b64, mem);

    char* decoded_buf = (char*) calloc(1, body_len + 1);
    int decoded_len = BIO_read(bio, decoded_buf, (int)body_len);
    
    BIO_free_all(bio);
    free(body_buf);

    if (decoded_len > 0)
    {
      decoded_buf[decoded_len] = '\0';
      return decoded_buf;
    }
    free(decoded_buf);
    return NULL;
  }

  return body_buf;
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
      curl_easy_cleanup(curl);
      curl_global_cleanup();
      return GOO_ERROR_FAILURE;
    }
 
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
                char**      body,
                char**      error)
{
  char inbox_url[4096] = {'\0'};
  snprintf(inbox_url, sizeof(inbox_url), "imaps://%s/INBOX", mail_imap_host);

  // 1. 查询收件箱中所有未读邮件的 UID
  char search_buf[4096] = {'\0'};
  int status = goo_mail_request(inbox_url, 
                                mail_imap_port,
                                mail_imap_user,
                                mail_imap_password,
                                "UID SEARCH UNSEEN",
                                goo_mail_search_cb,
                                search_buf,
                                error);
  if (status != GOO_SUCCESS)
    return status;

  // 2. 提取最后一个 UID (即最新的一封未读邮件)
  long uid = -1;
  char* token = strtok(search_buf, " \r\n");
  while (token != NULL)
  {
    long val = strtol(token, NULL, 10);
    if (val > 0)
    {
      uid = val; // 遍历覆盖，最终保留最后一个（最新的）UID
    }
    token = strtok(NULL, " \r\n");
  }

  // 没有未读邮件
  if (uid <= 0) 
  {
    if (error != NULL)
      *error = strdup("No unseen mail found");
    return GOO_ERROR_NO_MAIL;
  }

  printf("[INFO] 发现最新未读邮件，目标 UID: %ld\n", uid);

  char url[4096] = {'\0'};
  snprintf(url, sizeof(url), "imaps://%s/INBOX;uid=%ld", mail_imap_host, uid);
  
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
                            goo_mail_download,
                            (void*)fp,
                            error);
  fclose(fp);                 
  if (status != GOO_SUCCESS)
  {
    remove(tmpmail);
    return status;
  }

  char* mail_body = goo_mail_body(tmpmail);
  printf("%s\n", mail_body);

  // 4. 将该邮件在服务端标记为已读 (\Seen)
  char store_cmd[128] = {'\0'};
  snprintf(store_cmd, sizeof(store_cmd), "UID STORE %ld +FLAGS (\\Seen)", uid);
  goo_mail_request(inbox_url,
                   mail_imap_port,
                   mail_imap_user,
                   mail_imap_password,
                   store_cmd,
                   NULL,
                   NULL,
                   NULL);

  // 5. 解析邮件原始文件，提取附件
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
      *error = strdup("failed to open mail file for reading");
    fclose(fpb64);
    remove(tmpmail);
    remove(tmpatt);
    return GOO_ERROR_FAILURE;
  }

  char raw_attname[8192] = {'\0'};
  char attname[8192] = {'\0'};
  char line[1024];
  int in_attachment_header = 0;
  int start_attachment = 0;

  while (fgets(line, sizeof(line), fp) != NULL) 
  {
    // 进入附件头部区域判断（兼容顺序颠倒）
    if (strstr(line, "Content-Disposition: attachment") != NULL ||
        strstr(line, "Content-Type: application") != NULL)
    {
      in_attachment_header = 1;
    }

    // 在头部区域提取文件名（兼容换行及 name= 语法）
    if (in_attachment_header && raw_attname[0] == '\0')
    {
      const char* fn_pos = strstr(line, "filename=");
      if (fn_pos == NULL)
        fn_pos = strstr(line, "name=");

      if (fn_pos != NULL)
      {
        fn_pos += (strstr(line, "filename=") != NULL) ? 9 : 5;
        
        // 去除前面可能有的空格和双引号
        while (*fn_pos == ' ' || *fn_pos == '"' || *fn_pos == '\'')
          fn_pos++;

        int idx = 0;
        while (*fn_pos != '\0' && *fn_pos != '\r' && *fn_pos != '\n' && 
               *fn_pos != '"' && *fn_pos != ';' && *fn_pos != '\'')
        {
          raw_attname[idx++] = *fn_pos;
          fn_pos++;
        }
        raw_attname[idx] = '\0';
      }
    }

    // 遇到连续换行符，说明头部结束，接下来是 Base64 数据体
    if (in_attachment_header && (line[0] == '\r' || line[0] == '\n'))
    {
      start_attachment = 1;
      in_attachment_header = 0;
      continue;
    }

    // 写入 Base64 编码的数据
    if (start_attachment == 1)
    {
      if (strstr(line, "------") == line || strstr(line, "--") == line)
      {
        start_attachment = 0;
        break;
      }
      fwrite(line, 1, strlen(line), fpb64);
      fflush(fpb64);
    }
  }
  fclose(fp);
  fclose(fpb64);

  // 6. 将原始文件名还原（如果是编码过的原名，在此处解码）
  decode_mime_filename(attname, raw_attname);
  printf("[INFO] 成功提取到附件原始名称: [%s]\n", attname);

  // 7. 解码 Base64 附件并保存到指定下载目录
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

  // 8. 清理临时缓存文件
  remove(tmpmail);
  remove(tmpatt);

  return GOO_SUCCESS;
}