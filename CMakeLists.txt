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
##                                   poppler                                  ##
##                                                                            ##
################################################################################
set(POLLOPER_DIR                "3rd/poppler-25.10.0")
set(POLLOPER_INCLUDE_DIR        "${POLLOPER_DIR}"
                                "${POLLOPER_DIR}/poppler"
                                "${POLLOPER_DIR}/${BUILD}"
                                "${POLLOPER_DIR}/${BUILD}/poppler")
set(POLLOPER_LIBRARY_DIR        "${POLLOPER_DIR}/${BUILD}")
set(POLLOPER_LIBRARIES          poppler)

################################################################################
##                                                                            ##
##                                    xpdf                                    ##
##                                                                            ##
################################################################################
set(XPDF_DIR                "3rd/xpdf-4.05")
set(XPDF_INCLUDE_DIR        "${XPDF_DIR}/xpdf"
                            "${XPDF_DIR}/goo"
                            "${XPDF_DIR}/fofi"
                            "${XPDF_DIR}/splash"
                            "${XPDF_DIR}/${BUILD}")
set(XPDF_LIBRARY_DIR        "${XPDF_DIR}/${BUILD}/xpdf"
                            "${XPDF_DIR}/${BUILD}/splash"
                            "${XPDF_DIR}/${BUILD}/fofi"
                            "${XPDF_DIR}/${BUILD}/goo")
set(XPDF_LIBRARIES          xpdf fofi splash goo1)

################################################################################
##                                                                            ##
##                                   libpng                                   ##
##                                                                            ##
################################################################################
set(LIBPNG_DIR              "3rd/libpng-1.6.50")
set(LIBPNG_INCLUDE_DIR      "${LIBPNG_DIR}"
                            "${LIBPNG_DIR}/${BUILD}")
set(LIBPNG_LIBRARY_DIR      "${LIBPNG_DIR}/${BUILD}")
set(LIBPNG_LIBRARIES        png)

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
##                                   gfc                                      ##
##                                                                            ##
################################################################################
set(GFC_DIR                 "3rd/gfc-0.1.2")
set(GFC_INCLUDE_DIR         "${GFC_DIR}/include")
set(GFC_LIBRARY_DIR         "${GFC_DIR}/${BUILD}")
set(GFC_LIBRARIES           gfc)

################################################################################
##                                                                            ##
##                                    others                                  ##
##                                                                            ##
################################################################################
set(OTHER_LIBRARY_DIR       "/opt/homebrew/Cellar/fontconfig/2.17.1/lib")
set(OTHER_LIBRARIES         z fontconfig)

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
  "src/goolib-png.c"
  "src/internal/smtp.c"
)

include_directories(
  "src"
  ${OPENSSL_INCLUDE_DIR}
  ${LIBSSH2_INCLUDE_DIR}
  ${ARGPARSE_INCLUDE_DIR}
  ${MINIZIP_INCLUDE_DIR}
  ${JVM_INCLUDE_DIR}
  ${CJSON_INCLUDE_DIR}
  ${GFC_INCLUDE_DIR}
  ${XPDF_INCLUDE_DIR}
  ${LIBPNG_INCLUDE_DIR}
)

link_directories(
  ${LIBSSH2_LIBRARY_DIR}
  ${OPENSSL_LIBRARY_DIR}
  ${ARGPARSE_LIBRARY_DIR}
  ${MINIZIP_LIBRARY_DIR}
  ${CURL_LIBRARY_DIR}
  ${JVM_LIBRARY_DIR}
  ${CJSON_LIBRARY_DIR}
  ${XPDF_LIBRARY_DIR}
  ${LIBPNG_LIBRARY_DIR}
  ${GFC_LIBRARY_DIR}
  ${OTHER_LIBRARY_DIR}
)

add_library(goo STATIC ${GOOLIB_SOURCES})
add_library(goo_shared SHARED ${GOOLIB_SOURCES})
set_target_properties(goo_shared PROPERTIES OUTPUT_NAME goo)

target_link_libraries(goo_shared
  ${LIBSSH2_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  ${MINIZIP_LIBRARIES}
  ${CURL_LIBRARIES}
  ${XPDF_LIBRARIES}
  ${CJSON_LIBRARIES}
  ${LIBPNG_LIBRARIES}
  ${JVM_LIBRARIES}
  ${GFC_LIBRARIES}
  ${OTHER_LIBRARIES}
)

################################################################################
##                                                                            ##
##                                    tools                                   ##
##                                                                            ##
################################################################################

add_executable(goo-png-grayscale
  "tool/goo-png-grayscale.c"
)

target_link_libraries(goo-png-grayscale
  goo
  ${LIBPNG_LIBRARIES}
  ${ARGPARSE_LIBRARIES}
)

add_executable(goo-png-color
  "tool/goo-png-color.c"
)

target_link_libraries(goo-png-color
  goo
  ${LIBPNG_LIBRARIES}
  # ${OPENSSL_LIBRARIES}
  ${ARGPARSE_LIBRARIES}
)

add_executable(goo-png-crop
  "tool/goo-png-crop.c"
)

target_link_libraries(goo-png-crop
  goo
  ${LIBPNG_LIBRARIES}
  ${ARGPARSE_LIBRARIES}
)

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

add_executable(goo-proc-execute
  "tool/goo-proc-execute.c"
)

target_link_libraries(goo-proc-execute
  goo
  ${ARGPARSE_LIBRARIES}
)

add_executable(goo-proc-copy
  "tool/goo-proc-copy.c"
)

target_link_libraries(goo-proc-copy
  goo
  ${ARGPARSE_LIBRARIES}
  ${GFC_LIBRARIES}
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

add_executable(goo-pdf-crop
  "tool/goo-pdf-crop.c"
)

target_link_libraries(goo-pdf-crop
  goo
  ${ARGPARSE_LIBRARIES}
  ${XPDF_LIBRARIES}
  ${LIBPNG_LIBRARIES}
  ${GFC_LIBRARIES} 
  ${CJSON_LIBRARIES}
)

