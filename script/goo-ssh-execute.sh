export DYLD_LIBRARY_PATH=/Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/3rd/openssl-3.6.0/build/darwin/:/Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/3rd/curl-8.16.0/build/darwin/lib:/Users/christian/export/local/works/doublegsoft.open/goolib/03.Development/goolib/3rd/libssh2-1.11.1/build/darwin/src/.libs:$DYLD_LIBRARY_PATH

build/darwin/goo-ssh-execute -s 192.168.0.209 -p 22 -u christian -a ganguo \
  -c 'ls /Users/christian/Downloads'
