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
#ifndef GOOLIB_PG_H
#define GOOLIB_PG_H

#ifdef __cplusplus
extern "C" {
#endif

int
goo_pg_connect(const char* host, 
               const char* port, 
               const char* user, 
               const char* password, 
               const char* database,
               char**      error);

int
goo_pg_disconnect(int conn, 
                  const char* database,
                  char**      error);

int
goo_pg_query(int conn, 
             const char* query, 
             const char* output,
             char**      error);

#ifdef __cplusplus
}
#endif

#endif /* GOOLIB_PG_H */
