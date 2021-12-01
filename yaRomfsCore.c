/*
* Copyright (c) 2021 Erik Friesen
* 
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
* 
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
* LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>
#include "http/httpROMFS.h"

extern uint32_t readFileData(uint8_t* address, uint8_t* data, uint32_t length);

YAROMFSFILE_HANDLE *yaromfs_fopen(uint8_t *url, uint8_t *method, uint8_t *restData, uint32_t length) {
  char *urlParams = strstr(url, "?");
  if (urlParams) {
    *urlParams++ = 0;
  }
  const _yaROMFSFILE *handle = yaROMFSfind(url, method);
  if (handle == NULL) {
    return NULL;
  }
  YAROMFSFILE_HANDLE *ret = calloc(1, sizeof(YAROMFSFILE_HANDLE));
  if (ret) {
    ret->file = handle;
    ret->position = 0;
    if (handle->script_ptr != NULL) {
      ret->scriptResponse = handle->script_ptr(restData, length, urlParams);
      if (ret->scriptResponse.sendFile) {
        handle = yaROMFSfind(ret->scriptResponse.data, method);
        if (ret->scriptResponse.data) {
          free(ret->scriptResponse.data);
        }
        if (handle == NULL) {
          free(ret);
          return NULL;
        }
        memset(ret, 0, sizeof(ret));
        ret->file = handle;
        ret->position = 0;
      }
    }
  }
  return ret;
}

uint32_t yaromfs_fread(YAROMFSFILE_HANDLE *handle, uint8_t *buf, uint32_t len) {
  uint32_t ret;
  uint32_t remaining;
  if (handle->file->script_ptr != NULL) {
    /* Handle script file */
    remaining = handle->scriptResponse.len - handle->position;
    if (len > remaining) {
      len = remaining;
    } else if (len == 0) {
      return 0;
    }
    memcpy(buf, &handle->scriptResponse.data[handle->position], len);
    handle->position += len;
    return len;
  } else {
    remaining = handle->file->length - handle->position;
    uint8_t *flashAddress = (uint8_t *)handle->file->data;
    flashAddress += handle->position;
    if (len > remaining) {
      len = remaining;
    } else if (len == 0) {
      return 0;
    }
    if (ret = readFileData(flashAddress, buf, len)) {
      return 0;
    }
    handle->position += len;
    return len;
  }
}

uint32_t yaromfs_f_length(YAROMFSFILE_HANDLE *handle) {
  if (handle->file->script_ptr != NULL) {
    return handle->scriptResponse.len;
  } else {
    return handle->file->length;
  }
}

void yaromfs_fclose(YAROMFSFILE_HANDLE *handle) {
  if (handle != NULL) {
    if (handle->file->script_ptr != NULL) {
      if (handle->scriptResponse.data) {
        free(handle->scriptResponse.data);
      }
    }
    free(handle);
  }
}

uint32_t yaromfs_is_gz(YAROMFSFILE_HANDLE *handle) {
  if (handle->file->script_ptr != NULL) {
    return handle->scriptResponse.gz;
  } else {
    return handle->file->gz;
  }
}

const uint8_t * yaromfs_redirect(YAROMFSFILE_HANDLE *handle){
  return handle->file->contentType;
}

const uint8_t * yaromfs_contentType(YAROMFSFILE_HANDLE *handle){
  return handle->file->contentType;
}

uint32_t yaromfs_responseCode(YAROMFSFILE_HANDLE *handle) {
  if (handle->file->script_ptr != NULL) {
    return handle->scriptResponse.responseCode;
  } else {
    return 200;
  }
}

uint32_t yaromfs_postExists(uint8_t *url) {
  char *urlParams = strtok(url, "?");
  const _yaROMFSFILE *handle = yaROMFSfind(url, "POST");
  if (handle == NULL) {
    return 0;
  }
  return handle->length;
}

uint32_t yaromfs_preservePost(YAROMFSFILE_HANDLE *handle) {
  return handle->scriptResponse.preservePOST;
}