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
#ifndef GOOLIB_MAIL_H
#define GOOLIB_MAIL_H

#ifdef __cplusplus
extern "C" {
#endif

/*!
** Sends an email via SMTP.
**
** This function connects to the specified SMTP server, authenticates with the provided
** credentials, and attempts to send an email. It supports optional file attachments.
**
** @param mail_smtp_host     The hostname or IP address of the SMTP server.
** @param mail_smtp_port     The port number of the SMTP server (e.g., "587" or "465").
** @param mail_smtp_user     The username used for SMTP authentication.
** @param mail_smtp_password The password associated with the SMTP user.
** @param from               The sender's email address.
** @param to                 The recipient's email address.
** @param subject            The subject line of the email.
** @param body               The text content of the email body.
** @param attachment         The file path to an attachment, or NULL if no attachment is needed.
** @param error              Pointer to a char* that will point to an allocated error message 
**                           string if the operation fails. The caller is responsible for freeing 
**                           this memory. Pass NULL if error messages are not required.
**
** @return int               Returns 0 on success, or a non-zero error code on failure.
*/
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
              char**      error);

/*!
** Retrieves and downloads the latest email from the specified IMAP server.
**
** This function connects to an IMAP server, authenticates, locates the most recent email,
** and processes it. Attachments or messages may be stored in the specified directories.
**
** @param mail_imap_host     The hostname or IP address of the IMAP server.
** @param mail_imap_port     The port number of the IMAP server (e.g., "993").
** @param mail_imap_user     The username used for IMAP authentication.
** @param mail_imap_password The password associated with the IMAP user.
** @param temp_dir           The directory path used for storing temporary files during retrieval.
** @param download_dir       The destination directory where the retrieved email content or 
**                           attachments will be saved.
** @param error              Pointer to a char* that will point to an allocated error message 
**                           string if the operation fails. The caller is responsible for freeing 
**                           this memory. Pass NULL if error messages are not required.
**
** @return int               Returns 0 on success, or a non-zero error code on failure.
*/
int 
goo_mail_latest(const char* mail_imap_host,
                const char* mail_imap_port,
                const char* mail_imap_user,
                const char* mail_imap_password,
                const char* temp_dir,
                const char* download_dir,
                char**      error);

#ifdef __cplusplus
}
#endif

#endif /* GOOLIB_MAIL_H */