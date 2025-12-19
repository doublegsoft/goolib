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
#include "goolib-ssh.h"

int goo_ssh_execute(const char*   host, 
                    int           port,
                    const char*   user, 
                    const char*   password,
                    const char*   command,
                    char**        out,
                    char**        err)
{
  uint32_t hostaddr = inet_addr(host);
  libssh2_socket_t sock;
  int i;
  struct sockaddr_in sin;
  int rc;
  LIBSSH2_SESSION *session = NULL;
  LIBSSH2_CHANNEL *channel;

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

  if(libssh2_userauth_password(session, user, password)) {
    fprintf(stderr, "Authentication by password failed.\n");
    goto SHUTDOWN;
  }

  channel = libssh2_channel_open_session(session);
  if(!channel) 
  {
    fprintf(stderr, "Unable to open session\n");
    goto SHUTDOWN;
  }

  rc = libssh2_channel_exec(channel, command);
  if(rc) 
  {
    fprintf(stderr, "Unable to execute command: %d\n", rc);
    goto SHUTDOWN;
  }

  char buffer[1024];
  ssize_t bytes_read;
  size_t out_len;
  size_t err_len;

  FILE* out_st = open_memstream(out, &out_len);
  FILE* err_st = open_memstream(err, &err_len);

  while ((bytes_read = libssh2_channel_read(channel, buffer, sizeof(buffer))) > 0) 
  {
    fwrite(buffer, 1, bytes_read, out_st);
  }

  while ((bytes_read = libssh2_channel_read_stderr(channel, buffer, sizeof(buffer))) > 0) 
  {
    fwrite(buffer, 1, bytes_read, err_st);
  }
  fclose(out_st);
  fclose(err_st);

  libssh2_channel_close(channel);
  libssh2_channel_free(channel);

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