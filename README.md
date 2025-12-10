```
─────────────────────────────────────────────────────────────────────────────────────────
─██████████████─██████████████─██████████████─██████─────────██████████─██████████████───
─██░░░░░░░░░░██─██░░░░░░░░░░██─██░░░░░░░░░░██─██░░██─────────██░░░░░░██─██░░░░░░░░░░██───
─██░░██████████─██░░██████░░██─██░░██████░░██─██░░██─────────████░░████─██░░██████░░██───
─██░░██─────────██░░██──██░░██─██░░██──██░░██─██░░██───────────██░░██───██░░██──██░░██───
─██░░██─────────██░░██──██░░██─██░░██──██░░██─██░░██───────────██░░██───██░░██████░░████─
─██░░██──██████─██░░██──██░░██─██░░██──██░░██─██░░██───────────██░░██───██░░░░░░░░░░░░██─
─██░░██──██░░██─██░░██──██░░██─██░░██──██░░██─██░░██───────────██░░██───██░░████████░░██─
─██░░██──██░░██─██░░██──██░░██─██░░██──██░░██─██░░██───────────██░░██───██░░██────██░░██─
─██░░██████░░██─██░░██████░░██─██░░██████░░██─██░░██████████─████░░████─██░░████████░░██─
─██░░░░░░░░░░██─██░░░░░░░░░░██─██░░░░░░░░░░██─██░░░░░░░░░░██─██░░░░░░██─██░░░░░░░░░░░░██─
─██████████████─██████████████─██████████████─██████████████─██████████─████████████████─
```


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