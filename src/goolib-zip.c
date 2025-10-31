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
#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <mz.h>
#include <mz_os.h>
#include <mz_zip.h>
#include <mz_strm.h>
#include <mz_zip_rw.h>


#include "goolib-error.h"
#include "goolib-zip.h"

static void zip_add_dir(void* zip_handle, const char* dirpath, const char* base)
{
  DIR *dir = opendir(dirpath);
  if (!dir) {
    fprintf(stderr, "Cannot open directory %s\n", dirpath);
    return;
  }

  struct dirent *entry;
  char fullpath[1024];
  char relpath[1024];

  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, entry->d_name);

    struct stat st;
    stat(fullpath, &st);

    if (S_ISDIR(st.st_mode)) 
    {
      snprintf(relpath, sizeof(relpath), "%s/%s/", base, entry->d_name);
      mz_zip_writer_add_path(zip_handle, relpath, NULL, 0, 1);
      snprintf(relpath, sizeof(relpath), "%s/%s/", base, entry->d_name);
      zip_add_dir(zip_handle, fullpath, relpath);
    } 
    else if (S_ISREG(st.st_mode))
    {
      snprintf(relpath, sizeof(relpath), "%s/%s", base, entry->d_name);
      if (mz_zip_writer_add_file(zip_handle, fullpath, relpath) != MZ_OK)
        fprintf(stderr, "Failed to add %s\n", fullpath);
    }
  }

  closedir(dir);
}

int 
goo_zip_dir(const char* dir, const char* zip, char** error)
{

  FILE *out = fopen(zip, "wb");
  if (!out) 
  {
    *error = (char*)malloc(64 + strlen(zip));
    snprintf(*error, 128, "Failed to open file %s", zip);
    return GOO_ERROR_FAILURE;
  }

  void* zip_handle = NULL;
  int32_t err = MZ_OK;

  zip_handle = mz_zip_writer_create();
  if (zip_handle == NULL) 
  {
    *error = (char*)malloc(64);
    sprintf(*error, "failed to create zip writer\n");
    return GOO_ERROR_FAILURE;
  }

  err = mz_zip_writer_open_file(zip_handle, zip, 0, 0);
  if (err != MZ_OK) 
  {
    *error = (char*)malloc(64 + strlen(zip));
    sprintf(*error, "failed to open output file %s\n", zip);
    mz_zip_writer_delete(&zip_handle);
    return GOO_ERROR_FAILURE;
  }

  char* base = basename(dir);
  zip_add_dir(zip_handle, dir, base);
  free(base);

  mz_zip_writer_close(zip_handle);
  mz_zip_writer_delete(&zip_handle);

  return GOO_SUCCESS;
}