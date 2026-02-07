```                                                     
                               ▄▄▄▄         ██     ▄▄       
                               ▀▀██         ▀▀     ██       
  ▄███▄██   ▄████▄    ▄████▄     ██       ████     ██▄███▄  
 ██▀  ▀██  ██▀  ▀██  ██▀  ▀██    ██         ██     ██▀  ▀██ 
 ██    ██  ██    ██  ██    ██    ██         ██     ██    ██ 
 ▀██▄▄███  ▀██▄▄██▀  ▀██▄▄██▀    ██▄▄▄   ▄▄▄██▄▄▄  ███▄▄██▀ 
  ▄▀▀▀ ██    ▀▀▀▀      ▀▀▀▀       ▀▀▀▀   ▀▀▀▀▀▀▀▀  ▀▀ ▀▀▀   
  ▀████▀▀                                                                                      
```

# goolib

Small C/C++ utility library and CLI tools for everyday automation tasks: HTTP requests, SMTP email, SSH/SFTP, ZIP, PNG/PDF processing (including OCR), XLSX read/write, JVM interop, and process helpers.

The project builds a shared/static library `libgoo` and a set of `goo-*` command-line tools.

## Features

- Mail (SMTP/IMAP helpers): send emails with attachments via TLS (internal SMTP client). Tools: `goo-mail-send`, `goo-mail-latest`.
- HTTP: simple HTTP interactions via libcurl. Tool: `goo-http-google` (example).
- SSH/SFTP: execute remote commands and upload files via libssh2. Tools: `goo-ssh-execute`, `goo-sftp-upload`.
- PNG: grayscale/color conversion and crop. Tools: `goo-png-grayscale`, `goo-png-color`, `goo-png-crop`.
- PDF: crop, text extraction, optional OCR via Tesseract/Leptonica. Tools: `goo-pdf-crop`, `goo-pdf-text`.
- XLSX: read/write spreadsheets via xlsxio and libxlsxwriter.
- ZIP: directory archiving via minizip. Tool: `goo-zip-dir`.
- JVM interop: launch Java code via JNI. Tool: `goo-jvm-java`.
- Process utils: run commands, copy files, build helpers. Tools: `goo-proc-*`.

See [src/](cci:7://file:///Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/src:0:0-0:0) for the C APIs (e.g., [goolib-mail.h](cci:7://file:///Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/src/goolib-mail.h:0:0-0:0), [goolib-pdf.h](cci:7://file:///Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/src/goolib-pdf.h:0:0-0:0), [goolib-png.h](cci:7://file:///Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/src/goolib-png.h:0:0-0:0), [goolib-xlsx.h](cci:7://file:///Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/src/goolib-xlsx.h:0:0-0:0), [goolib-ssh.h](cci:7://file:///Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/src/goolib-ssh.h:0:0-0:0), [goolib-http.h](cci:7://file:///Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/src/goolib-http.h:0:0-0:0), [goolib-proc.h](cci:7://file:///Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/src/goolib-proc.h:0:0-0:0), [goolib-zip.h](cci:7://file:///Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/src/goolib-zip.h:0:0-0:0), [goolib-jvm.h](cci:7://file:///Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/src/goolib-jvm.h:0:0-0:0)). Include headers via `#include "goolib.h"`.

## Project layout

- [src/](cci:7://file:///Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/src:0:0-0:0) Core library sources and headers
- [src/internal/](cci:7://file:///Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/src/internal:0:0-0:0) Internal SMTP client (public-domain CC0)
- [tool/](cci:7://file:///Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/tool:0:0-0:0) CLI tools built on top of libgoo
- [3rd/](cci:7://file:///Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/3rd:0:0-0:0) Vendored third-party dependencies (expect prebuilt artifacts under `3rd/*/build/darwin` by default)
- [script/](cci:7://file:///Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/script:0:0-0:0) Convenience shell wrappers/examples

## Build requirements

- CMake >= 3.13, C11/C++17 compiler
- Third-party libraries (expected paths are configured in [CMakeLists.txt](cci:7://file:///Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/CMakeLists.txt:0:0-0:0)):
  - OpenSSL 3.6.0
  - libcurl 8.16.0
  - libssh2 1.11.1
  - minizip-ng 4.0.10
  - xpdf 4.05
  - poppler 25.10.0 (headers for PDF support)
  - Tesseract + Leptonica (for OCR in `goo-pdf-text`)
  - libpng 1.6.50
  - cJSON 1.7.19
  - xlsxio 0.2.36
  - libxlsxwriter 1.2.3
  - FreeType (for some PDF/text rendering scenarios)
  - Fontconfig, zlib, libarchive (as linked in `OTHER_LIBRARIES`)

Paths are currently set for macOS (`PLATFORM=darwin`). Adjust paths in [CMakeLists.txt](cci:7://file:///Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/CMakeLists.txt:0:0-0:0) if your environment differs.

## Build (macOS example)

mkdir -p build/darwin
cmake -S . -B build/darwin
cmake --build build/darwin -j

This assumes required third-party libraries are already built and placed under the paths configured in [CMakeLists.txt](cci:7://file:///Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/CMakeLists.txt:0:0-0:0) (e.g., `3rd/*/build/darwin`). If not, build those first following each project’s documentation.

### Runtime dynamic libraries (macOS)

Some tools need OpenSSL/libcurl/libssh2 at runtime. Example environment setup:

export DYLD_LIBRARY_PATH=/Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/3rd/openssl-3.6.0/build/darwin:/Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/3rd/curl-8.16.0/build/darwin/lib:/Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/3rd/libssh2-1.11.1/build/darwin/src/.libs:$DYLD_LIBRARY_PATH

## Build (Windows cross-compile via MinGW on Ubuntu)

Create a `toolchain-mingw64.cmake` like:

set(CMAKE_SYSTEM_NAME Windows)
set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_Fortran_COMPILER ${TOOLCHAIN_PREFIX}-gfortran)
set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)

set(CMAKE_FIND_ROOT_PATH /usr/${TOOLCHAIN_PREFIX})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

Configure and build:

cmake -S . -B build/mingw64 -DCMAKE_TOOLCHAIN_FILE=toolchain-mingw64.cmake
cmake --build build/mingw64 -j

libcurl on MinGW may require `libpsl`. You can download from https://packages.msys2.org/packages/mingw-w64-x86_64-libpsl and place it under `/usr/x86_64-w64-mingw32/` if needed:

sudo cp -rf /path/to/mingw-w64-x86_64-libpsl/* /usr/x86_64-w64-mingw32/

## CLI tools and examples

- goo-mail-send: send an email (TLS via SMTP)
  goo-mail-send \
    --smtp-host smtp.example.com \
    --smtp-port 465 \
    --smtp-user user@example.com \
    --smtp-password 'secret' \
    --mail-from user@example.com \
    --mail-to someone@example.com \
    --mail-subject "Hello" \
    --mail-body "<p>Hi!</p>" \
    --mail-file /path/to/optional-attachment

- goo-mail-latest: fetch latest mail/attachments (IMAP)
  goo-mail-latest \
    --imap-host imap.example.com \
    --imap-port 993 \
    --imap-user user@example.com \
    --imap-password 'secret' \
    --temp-dir /tmp \
    --download-dir /path/to/save

- goo-png-grayscale | goo-png-color | goo-png-crop: PNG utilities

- goo-pdf-crop: crop a PDF via xpdf; goo-pdf-text: extract text (with OCR fallback when enabled). See [script/](cci:7://file:///Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/script:0:0-0:0) for usage examples.

- goo-ssh-execute | goo-sftp-upload: SSH/SFTP helpers.

- goo-zip-dir: zip a directory using minizip.

- goo-jvm-java: run Java code via JNI (requires JVM path in [CMakeLists.txt](cci:7://file:///Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/CMakeLists.txt:0:0-0:0)).

Each tool supports -h/--help for usage. Wrapper scripts in [script/](cci:7://file:///Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/script:0:0-0:0) show common invocations.

## Library usage

Include the umbrella header and link against libgoo:

#include "goolib.h"

int send_demo(void) {
  char* err = NULL;
  int rc = goo_mail_send("smtp.example.com", "465",
                         "user@example.com", "secret",
                         "user@example.com", "to@example.com",
                         "Subject", "Body", NULL, &err);
  if (rc != 0) {
    fprintf(stderr, "Error: %s\n", err);
    free(err);
  }
  return rc;
}

## Notes for third-party builds

- libssh2 1.11.1 can be configured against OpenSSL (example):
  ../../configure --with-libssl-prefix=/Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/3rd/openssl-3.6.0
- Ensure your built artifacts land in the paths referenced by CMakeLists.txt (e.g., ${PROJECT_ROOT}/3rd/NAME/build/darwin/...).

## Acknowledgements

This project vendors or links against:

- OpenSSL, libcurl, libssh2, minizip-ng, xpdf, poppler, Tesseract, Leptonica, libpng, cJSON, xlsxio, libxlsxwriter, FreeType, Fontconfig, zlib, libarchive, and others. Licenses are held by their respective owners.
- SMTP client under src/internal/smtp.* is public domain (CC0).

## Status

Actively evolving; APIs and paths may change. Review CMakeLists.txt before building on a new machine.
EOF

If you’d prefer, I can instead create a new README_FULL.md for review before touching the existing file. Let me know your preference.

Summary:
- I analyzed the project structure, dependencies, and tools.
- Drafted a comprehensive README that preserves your original environment notes.
- Ready to append it to README.md upon your approval.


#### libssh2-1.11.1

```
../../configure --with-libssl-prefix=/Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/lib/darwin-arm64/openssl-3.6.0
```

### goo-mail-send

```
export DYLD_LIBRARY_PATH=/Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/3rd/openssl-3.6.0/build/darwin/:/Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/3rd/curl-8.16.0/build/darwin/lib:/Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/3rd/libssh2-1.11.1/build/darwin/src/.libs:$DYLD_LIBRARY_PATH
```

## Build Windows Version Using MinGW on Ubuntu

### CMake MinGW Tool-Chain

```
set(CMAKE_SYSTEM_NAME Windows)
set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)

# cross compilers to use for C, C++ and Fortran
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_Fortran_COMPILER ${TOOLCHAIN_PREFIX}-gfortran)
set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)

# target environment on the build host system
set(CMAKE_FIND_ROOT_PATH /usr/${TOOLCHAIN_PREFIX})

# modify default behavior of FIND_XXX() commands
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```

```
cmake ../.. -DCMAKE_TOOLCHAIN_FILE=../toolchain-mingw64.cmake
```

从此处[mingw-w64-x86_64-libpsl](https://packages.msys2.org/packages/mingw-w64-x86_64-libpsl)下载，解压后复制到/usr/x86_64-w64-mingw32目录中。

```
cp -rf /path/to/mingw-w64-x86_64-libpsl/* /usr/x86_64-w64-mingw32/
```