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
#include <libssh2.h>
#include <libssh2_sftp.h>

#ifdef _WIN32
#define write(f, b, c)  write((f), (b), (unsigned int)(c))
#endif
 
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

#include "goolib-error.h"

int 
goo_sftp_upload(const char* host, 
                int port,
                const char* username, 
                const char* password, 
                const char* local_path, 
                const char* remote_path, 
                char** error)
{
  uint32_t hostaddr = inet_addr(host);
  libssh2_socket_t sock;
  int i;
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