## ─────────────────────────────────────────────────────────────────────────────────────────
## ─██████████████─██████████████─██████████████─██████─────────██████████─██████████████───
## ─██░░░░░░░░░░██─██░░░░░░░░░░██─██░░░░░░░░░░██─██░░██─────────██░░░░░░██─██░░░░░░░░░░██───
## ─██░░██████████─██░░██████░░██─██░░██████░░██─██░░██─────────████░░████─██░░██████░░██───
## ─██░░██─────────██░░██──██░░██─██░░██──██░░██─██░░██───────────██░░██───██░░██──██░░██───
## ─██░░██─────────██░░██──██░░██─██░░██──██░░██─██░░██───────────██░░██───██░░██████░░████─
## ─██░░██──██████─██░░██──██░░██─██░░██──██░░██─██░░██───────────██░░██───██░░░░░░░░░░░░██─
## ─██░░██──██░░██─██░░██──██░░██─██░░██──██░░██─██░░██───────────██░░██───██░░████████░░██─
## ─██░░██──██░░██─██░░██──██░░██─██░░██──██░░██─██░░██───────────██░░██───██░░██────██░░██─
## ─██░░██████░░██─██░░██████░░██─██░░██████░░██─██░░██████████─████░░████─██░░████████░░██─
## ─██░░░░░░░░░░██─██░░░░░░░░░░██─██░░░░░░░░░░██─██░░░░░░░░░░██─██░░░░░░██─██░░░░░░░░░░░░██─
## ─██████████████─██████████████─██████████████─██████████████─██████████─████████████████─
cmake_minimum_required(VERSION 3.13)

project(goolib C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(PLATFORM                    "darwin")
set(BUILD                       "build/${PLATFORM}")

################################################################################
##                                                                            ##
##                                   argparse                                 ##
##                                                                            ##
################################################################################
set(ARGPARSE_DIR                "3rd/argparse-1.1.0")
set(ARGPARSE_INCLUDE_DIR        "${ARGPARSE_DIR}")
set(ARGPARSE_LIBRARY_DIR        "${ARGPARSE_DIR}/${BUILD}")
set(ARGPARSE_LIBRARIES          argparse)

################################################################################
##                                                                            ##
##                                   minizip                                  ##
##                                                                            ##
################################################################################
set(MINIZIP_DIR                 "3rd/minizip-ng-4.0.10")
set(MINIZIP_INCLUDE_DIR         "${MINIZIP_DIR}")
set(MINIZIP_LIBRARY_DIR         "${MINIZIP_DIR}/${BUILD}")
set(MINIZIP_LIBRARIES           minizip)

################################################################################
##                                                                            ##
##                                    curl                                    ##
##                                                                            ##
################################################################################
set(CURL_DIR                    "3rd/curl-8.16.0")
set(CURL_INCLUDE_DIR            "${CURL_DIR}/include")
set(CURL_LIBRARY_DIR            "${CURL_DIR}/${BUILD}/lib")
set(CURL_LIBRARIES              curl)

################################################################################
##                                                                            ##
##                                   libssh2                                  ##
##                                                                            ##
################################################################################
set(LIBSSH2_DIR                 "3rd/libssh2-1.11.1")
set(LIBSSH2_INCLUDE_DIR         "${LIBSSH2_DIR}/include")
set(LIBSSH2_LIBRARY_DIR         "${LIBSSH2_DIR}/${BUILD}/src/.libs")
set(LIBSSH2_LIBRARIES           ssh2)

################################################################################
##                                                                            ##
##                                   openssl                                  ##
##                                                                            ##
################################################################################
set(OPENSSL_DIR                 "3rd/openssl-3.6.0")
set(OPENSSL_INCLUDE_DIR         "${OPENSSL_DIR}/include"
                                "${OPENSSL_DIR}/${BUILD}/include")
set(OPENSSL_LIBRARY_DIR         "${OPENSSL_DIR}/${BUILD}")
set(OPENSSL_LIBRARIES           crypto ssl)

################################################################################
##                                                                            ##
##                                    cjson                                   ##
##                                                                            ##
################################################################################
set(CJSON_DIR                   "3rd/cJSON-1.7.19")
set(CJSON_INCLUDE_DIR           "${CJSON_DIR}")
set(CJSON_LIBRARY_DIR           "${CJSON_DIR}/${BUILD}")
set(CJSON_LIBRARIES             cjson)

################################################################################
##                                                                            ##
##                                   jvm                                      ##
##                                                                            ##
################################################################################
set(JVM_DIR                 "/Users/christian/export/opt/jdk-21.0.8/Contents/Home")
set(JVM_INCLUDE_DIR         "${JVM_DIR}/include"
                            "${JVM_DIR}/include/${PLATFORM}")
set(JVM_LIBRARY_DIR         "${JVM_DIR}/lib/server")
set(JVM_LIBRARIES           jvm)

################################################################################
##                                                                            ##
##                                    others                                  ##
##                                                                            ##
################################################################################

set(OTHER_LIBRARIES         z)

################################################################################
##                                                                            ##
##                                   goolib                                   ##
##                                                                            ##
################################################################################
set(GOOLIB_SOURCES
  "src/goolib-util.c"
  "src/goolib-error.c"
  "src/goolib-mail.c"
  "src/goolib-zip.c"
  "src/goolib-sftp.c"
  "src/goolib-ssh.c"
  "src/goolib-jvm.c"
  "src/goolib-proc.c"
  "src/internal/smtp.c"
)

include_directories(
  "src"
  ${OPENSSL_INCLUDE_DIR}
  ${LIBSSH2_INCLUDE_DIR}
  ${ARGPARSE_INCLUDE_DIR}
  ${MINIZIP_INCLUDE_DIR}
  ${JVM_INCLUDE_DIR}
)

link_directories(
  ${LIBSSH2_LIBRARY_DIR}
  ${OPENSSL_LIBRARY_DIR}
  ${ARGPARSE_LIBRARY_DIR}
  ${MINIZIP_LIBRARY_DIR}
  ${CURL_LIBRARY_DIR}
  ${JVM_LIBRARY_DIR}
)

add_library(goo STATIC ${GOOLIB_SOURCES})
add_library(goo_shared SHARED ${GOOLIB_SOURCES})
set_target_properties(goo_shared PROPERTIES OUTPUT_NAME goo)

target_link_libraries(goo_shared
  ${LIBSSH2_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  ${MINIZIP_LIBRARIES}
  ${CURL_LIBRARIES}
  ${JVM_LIBRARIES}
  ${OTHER_LIBRARIES}
)

################################################################################
##                                                                            ##
##                                    tools                                   ##
##                                                                            ##
################################################################################

add_executable(goo-mail-send
  "tool/goo-mail-send.c"
)

target_link_libraries(goo-mail-send
  goo
  ${OPENSSL_LIBRARIES}
  ${ARGPARSE_LIBRARIES}
  ${CURL_LIBRARIES}
)

add_executable(goo-mail-latest
  "tool/goo-mail-latest.c"
)

target_link_libraries(goo-mail-latest
  goo
  ${OPENSSL_LIBRARIES}
  ${ARGPARSE_LIBRARIES}
  ${CURL_LIBRARIES}
)

add_executable(goo-zip-dir
  "tool/goo-zip-dir.c"
)

target_link_libraries(goo-zip-dir
  goo
  ${ARGPARSE_LIBRARIES}
  ${MINIZIP_LIBRARIES}
  ${OTHER_LIBRARIES}
)

add_executable(goo-sftp-upload
  "tool/goo-sftp-upload.c"
)

target_link_libraries(goo-sftp-upload
  goo
  ${ARGPARSE_LIBRARIES}
  ${LIBSSH2_LIBRARIES}
)

add_executable(goo-ssh-execute
  "tool/goo-ssh-execute.c"
)

target_link_libraries(goo-ssh-execute
  goo
  ${ARGPARSE_LIBRARIES}
  ${LIBSSH2_LIBRARIES}
)

add_executable(goo-jvm-java
  "tool/goo-jvm-java.c"
)

target_link_libraries(goo-jvm-java
  goo
  ${ARGPARSE_LIBRARIES}
  ${JVM_LIBRARIES}
)

add_executable(goo-proc-javac
  "tool/goo-proc-javac.c"
)

target_link_libraries(goo-proc-javac
  goo
  ${ARGPARSE_LIBRARIES}
)

add_executable(goo-http-google
  "tool/goo-http-google.c"
)

target_link_libraries(goo-http-google
  goo
  ${CURL_LIBRARIES}
)

add_executable(goo-llm-ollama
  "tool/goo-llm-ollama.c"
)

target_link_libraries(goo-llm-ollama
  goo
  ${CURL_LIBRARIES}
)

add_executable(goo-llm-gemini
  "tool/goo-llm-gemini.c"
)

target_link_libraries(goo-llm-gemini
  goo
  ${CURL_LIBRARIES}
)

