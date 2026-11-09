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
#include <string.h>
#include <libssh2.h>
#include <libssh2_sftp.h>

#ifdef _WIN32
#define write(f, b, c)  write((f), (b), (unsigned int)(c))
#endif
 
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <io.h>
#include <direct.h>
#define stat _stat
#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & _S_IFDIR) == _S_IFDIR)
#endif
#ifndef S_ISREG
#define S_ISREG(mode) (((mode) & _S_IFREG) == _S_IFREG)
#endif
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#endif

#include "goolib-error.h"

/* 
** 辅助函数：在 SFTP 服务端递归创建多级目录 (类似 mkdir -p) 
*/
static int
sftp_mkdir_p(LIBSSH2_SFTP *sftp_session, const char *path)
{
  char tmp[1024];
  size_t len = strlen(path);
  if (len >= sizeof(tmp)) return -1;
  strcpy(tmp, path);

  // 统一路径分隔符为 '/'
  for (size_t i = 0; i < len; i++) {
    if (tmp[i] == '\\') tmp[i] = '/';
  }

  // 跳过绝对路径开头的 '/'
  size_t start = 0;
  if (tmp[0] == '/') {
    start = 1;
  }

  for (size_t i = start; i < len; i++) {
    if (tmp[i] == '/') {
      tmp[i] = '\0';
      // 逐层创建目录，如果已存在，libssh2_sftp_mkdir 会失败但无妨，继续向下
      libssh2_sftp_mkdir(sftp_session, tmp, 0755);
      tmp[i] = '/';
    }
  }
  // 创建最深的一层目录
  libssh2_sftp_mkdir(sftp_session, tmp, 0755);
  return 0;
}

/* 
** 辅助函数：在已有的 SFTP 会话下上传单个文件 
*/
static int
sftp_upload_file(LIBSSH2_SFTP *sftp_session, 
                 const char *local_path, 
                 const char *remote_path)
{
  LIBSSH2_SFTP_HANDLE *sftp_handle = libssh2_sftp_open(
      sftp_session, 
      remote_path,
      LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC,
      0644); // 默认读写权限
  
  if(!sftp_handle) {
    fprintf(stderr, "Unable to open remote file %s for writing\n", remote_path);
    return -1;
  }

  FILE* local_file = fopen(local_path, "rb");
  if(!local_file) {
    fprintf(stderr, "Unable to open local file %s for reading\n", local_path);
    libssh2_sftp_close(sftp_handle);
    return -1;
  }

  char buffer[4096];
  size_t nread;
  int success = 0;

  while ((nread = fread(buffer, 1, sizeof(buffer), local_file)) > 0) {
    ssize_t written = 0;
    while (written < nread) {
      ssize_t current_write = libssh2_sftp_write(sftp_handle, buffer + written, nread - written);
      if (current_write == LIBSSH2_ERROR_EAGAIN) {
        continue; 
      } 
      else if (current_write < 0) {
        fprintf(stderr, "Error writing to remote file: %ld\n", libssh2_sftp_last_error(sftp_session));
        success = -1;
        break;
      }
      written += current_write;
    }
    if (success < 0) break;
  }

  fclose(local_file);
  libssh2_sftp_close(sftp_handle);
  return success;
}

/* 
** 辅助函数：递归扫描本地目录，对应在 SFTP 上创建子目录并上传文件 
*/
static int
upload_dir_recursive(LIBSSH2_SFTP *sftp_session, 
                     const char *local_dir, 
                     const char *remote_dir)
{
  int success = 0;

#ifdef _WIN32
  char search_path[1024];
  snprintf(search_path, sizeof(search_path), "%s\\*", local_dir);
  WIN32_FIND_DATAA find_data;
  HANDLE hFind = FindFirstFileA(search_path, &find_data);
  if (hFind != INVALID_HANDLE_VALUE) {
    do {
      if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0) {
        continue;
      }
      char local_path[1024];
      snprintf(local_path, sizeof(local_path), "%s/%s", local_dir, find_data.cFileName);

      char remote_path[1024];
      snprintf(remote_path, sizeof(remote_path), "%s/%s", remote_dir, find_data.cFileName);

      if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        sftp_mkdir_p(sftp_session, remote_path);
        if (upload_dir_recursive(sftp_session, local_path, remote_path) < 0) {
          success = -1;
        }
      } else {
        if (sftp_upload_file(sftp_session, local_path, remote_path) < 0) {
          success = -1;
        }
      }
    } while (FindNextFileA(hFind, &find_data));
    FindClose(hFind);
  } else {
    success = -1;
  }
#else
  DIR *dir = opendir(local_dir);
  if (!dir) {
    fprintf(stderr, "Failed to open local directory: %s\n", local_dir);
    return -1;
  }
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }
    char local_path[1024];
    snprintf(local_path, sizeof(local_path), "%s/%s", local_dir, entry->d_name);

    char remote_path[1024];
    snprintf(remote_path, sizeof(remote_path), "%s/%s", remote_dir, entry->d_name);

    struct stat st;
    if (stat(local_path, &st) == 0) {
      if (S_ISDIR(st.st_mode)) {
        sftp_mkdir_p(sftp_session, remote_path);
        if (upload_dir_recursive(sftp_session, local_path, remote_path) < 0) {
          success = -1;
        }
      } else if (S_ISREG(st.st_mode)) {
        if (sftp_upload_file(sftp_session, local_path, remote_path) < 0) {
          success = -1;
        }
      }
    }
  }
  closedir(dir);
#endif

  return success;
}

/*
** 原有函数：SFTP 上传单个文件
*/
int 
goo_sftp_file(const char* host, 
              int port,
              const char* username, 
              const char* password, 
              const char* local_path, 
              const char* remote_path, 
              char** error)
{
  uint32_t hostaddr = inet_addr(host);
  libssh2_socket_t sock;
  struct sockaddr_in sin;
  int rc;
  LIBSSH2_SESSION *session = NULL;
  LIBSSH2_SFTP *sftp_session;
  LIBSSH2_SFTP_HANDLE *sftp_handle;

#ifdef _WIN32
  WSADATA wsadata;
  rc = WSAStartup(MAKEWORD(2, 0), &wsadata);
  if(rc) 
  {
    fprintf(stderr, "WSAStartup failed with error: %d\n", rc);
    return GOO_ERROR_FAILURE;
  }
#endif

  rc = libssh2_init(0);

  if(rc) 
  {
    fprintf(stderr, "libssh2 initialization failed (%d)\n", rc);
    return GOO_ERROR_FAILURE;
  }

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock == LIBSSH2_INVALID_SOCKET) 
  {
    fprintf(stderr, "failed to create socket.\n");
    return GOO_ERROR_FAILURE;
  }

  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  sin.sin_addr.s_addr = hostaddr;
  if(connect(sock, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in))) 
  {
    fprintf(stderr, "failed to connect.\n");
    return GOO_ERROR_FAILURE;
  }

  session = libssh2_session_init();

  if(!session) 
  {
    fprintf(stderr, "Could not initialize SSH session.\n");
    return GOO_ERROR_FAILURE;
  }

  libssh2_session_set_blocking(session, 1);
  rc = libssh2_session_handshake(session, sock);

  if(rc) 
  {
    fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
    return GOO_ERROR_FAILURE;
  }

  if(libssh2_userauth_password(session, username, password)) {
    fprintf(stderr, "Authentication by password failed.\n");
    goto SHUTDOWN;
  }

  sftp_session = libssh2_sftp_init(session);
  if(!sftp_session) 
  {
    fprintf(stderr, "Unable to init SFTP session\n");
    goto SHUTDOWN;
  }
 
  sftp_handle = libssh2_sftp_open(sftp_session, 
                                  remote_path,
                                  LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC,
                                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  
  FILE* local_file = fopen(local_path, "rb");
  char buffer[4096];
  size_t nread;
  while ((nread = fread(buffer, 1, sizeof(buffer), local_file)) > 0) {
    ssize_t written = 0;
    while (written < nread) 
    {
      ssize_t current_write = libssh2_sftp_write(sftp_handle, buffer + written, nread - written);
      if (current_write == LIBSSH2_ERROR_EAGAIN) 
      {
        continue; 
      } 
      else if (current_write < 0) 
      {
        fprintf(stderr, "Error writing to remote file: %ld\n", libssh2_sftp_last_error(sftp_session));
        goto SHUTDOWN;
      }
      written += current_write;
    }
  }                                  

  libssh2_sftp_close(sftp_handle);
  libssh2_sftp_shutdown(sftp_session);

SHUTDOWN:  
  if(session) 
  {
      libssh2_session_disconnect(session, "Normal Shutdown");
      libssh2_session_free(session);
  }
 
  if(sock != LIBSSH2_INVALID_SOCKET) 
  {
    shutdown(sock, 2);
    LIBSSH2_SOCKET_CLOSE(sock);
  }
 
  libssh2_exit();
  return GOO_SUCCESS;
} 

/*
** 新增函数：支持目录对目录的递归上传，自动创建远程各级子目录
*/
int 
goo_sftp_dir(const char* host, 
             int port,
             const char* username, 
             const char* password, 
             const char* local_dir, 
             const char* remote_dir, 
             char** error)
{
  uint32_t hostaddr = inet_addr(host);
  libssh2_socket_t sock;
  struct sockaddr_in sin;
  int rc;
  LIBSSH2_SESSION *session = NULL;
  LIBSSH2_SFTP *sftp_session = NULL;

#ifdef _WIN32
  WSADATA wsadata;
  rc = WSAStartup(MAKEWORD(2, 0), &wsadata);
  if(rc) 
  {
    fprintf(stderr, "WSAStartup failed with error: %d\n", rc);
    return GOO_ERROR_FAILURE;
  }
#endif

  rc = libssh2_init(0);
  if(rc) 
  {
    fprintf(stderr, "libssh2 initialization failed (%d)\n", rc);
    return GOO_ERROR_FAILURE;
  }

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock == LIBSSH2_INVALID_SOCKET) 
  {
    fprintf(stderr, "failed to create socket.\n");
    return GOO_ERROR_FAILURE;
  }

  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  sin.sin_addr.s_addr = hostaddr;
  if(connect(sock, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in))) 
  {
    fprintf(stderr, "failed to connect.\n");
    return GOO_ERROR_FAILURE;
  }

  session = libssh2_session_init();
  if(!session) 
  {
    fprintf(stderr, "Could not initialize SSH session.\n");
    return GOO_ERROR_FAILURE;
  }

  libssh2_session_set_blocking(session, 1);
  rc = libssh2_session_handshake(session, sock);
  if(rc) 
  {
    fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
    return GOO_ERROR_FAILURE;
  }

  if(libssh2_userauth_password(session, username, password)) {
    fprintf(stderr, "Authentication by password failed.\n");
    goto SHUTDOWN;
  }

  sftp_session = libssh2_sftp_init(session);
  if(!sftp_session) 
  {
    fprintf(stderr, "Unable to init SFTP session\n");
    goto SHUTDOWN;
  }

  // 1. 确保远程主目标目录存在 (包括缺少的上层目录)
  sftp_mkdir_p(sftp_session, remote_dir);

  // 2. 递归上传本地目录中的内容
  rc = upload_dir_recursive(sftp_session, local_dir, remote_dir);

  libssh2_sftp_shutdown(sftp_session);

SHUTDOWN:  
  if(session) 
  {
      libssh2_session_disconnect(session, "Normal Shutdown");
      libssh2_session_free(session);
  }
 
  if(sock != LIBSSH2_INVALID_SOCKET) 
  {
    shutdown(sock, 2);
    LIBSSH2_SOCKET_CLOSE(sock);
  }
 
  libssh2_exit();

  return (rc == 0) ? GOO_SUCCESS : GOO_ERROR_FAILURE;
}