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
#include <stdio.h>
#include <stdlib.h>
#include <argparse.h>

/* 引入跨平台 stat 库，用于判断本地路径属性 */
#ifdef _WIN32
#include <sys/stat.h>
#define stat _stat
#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & _S_IFDIR) == _S_IFDIR)
#endif
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "goolib-error.h"
#include "goolib-sftp.h"

static const char *const usages[] = 
{
  "goo-sftp-upload [options]",
  NULL,
};

int main(int argc, char* argv[]) 
{
  char* host = NULL;
  int port = 0;
  char* username = NULL;
  char* password = NULL;
  char* local = NULL;
  char* remote = NULL;

  struct argparse_option options[] = {
    OPT_HELP(),
    OPT_STRING('s', "server", &host, "ssh server address", NULL, 0, 0),
    OPT_INTEGER('p', "port", &port, "ssh port, 22", NULL, 0, 0),
    OPT_STRING('u', "username", &username, "account name", NULL, 0, 0),
    OPT_STRING('a', "password", &password, "account password", NULL, 0, 0),
    OPT_STRING('l', "local", &local, "local file/directory path", NULL, 0, 0),
    OPT_STRING('r', "remote", &remote, "remote file/directory path", NULL, 0, 0),
    OPT_END(),
  };

  struct argparse argparse;
  argparse_init(&argparse, options, usages, 0);
  argparse_describe(&argparse, "\nUpload file or directory via SFTP.", NULL);
  
  argc = argparse_parse(&argparse, argc, (const char**) argv);

  if (host == NULL || 
      port == 0 || 
      username == NULL || 
      password == NULL || 
      local == NULL || 
      remote == NULL ) 
  {
    argparse_usage(&argparse);
    return GOO_ERROR_FAILURE;
  }

  /* 1. 检查本地路径是否存在以及获取路径属性 */
  struct stat st;
  if (stat(local, &st) != 0)
  {
    fprintf(stderr, "Error: Local path '%s' does not exist or is inaccessible.\n", local);
    return GOO_ERROR_FAILURE;
  }

  char* error = NULL;
  int rc;

  /* 2. 根据路径属性，分流调用不同的上传函数 */
  if (S_ISDIR(st.st_mode))
  {
    // 如果是目录，调用目录递归上传函数
    rc = goo_sftp_dir(host, port, 
                      username, password, 
                      local, remote, 
                      &error);
  }
  else
  {
    // 如果是普通文件，调用原有的单文件上传函数
    rc = goo_sftp_file(host, port, 
                       username, password, 
                       local, remote, 
                       &error);
  }

  if (rc != GOO_SUCCESS)
  {
    fprintf(stderr, "Error: %s\n", error);
    if (error != NULL)
      free(error);
    return GOO_ERROR_FAILURE;
  }

  return GOO_SUCCESS; 
}