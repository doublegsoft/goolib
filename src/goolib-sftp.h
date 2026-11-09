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
#ifndef GOOLIB_SFTP_H
#define GOOLIB_SFTP_H

#ifdef __cplusplus
extern "C" {
#endif

int 
goo_sftp_file(const char* host, 
              int port,
              const char* username, 
              const char* password, 
              const char* local_path, 
              const char* remote_path, 
              char** error);

int 
goo_sftp_dir(const char* host, 
             int port,
             const char* username, 
             const char* password, 
             const char* local_dir, 
             const char* remote_dir, 
             char** error);                

#ifdef __cplusplus
}
#endif

#endif /* GOOLIB_SFTP_H */
  