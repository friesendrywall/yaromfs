## yaromfs

This the source for building the a read only file system for a customized FreeRTOS HTTP webserver.

## Build

Windows build using MINGW.

## Usage

This is built using input files rather than params.
An example usage would be:

```myromfs web-source.txt```

web-source.txt example

```
[WEBROOT]
./web/build
[OUTPUT_SRC]
./firmware/src/http
[OUTPUT_HDR]
./firmware/inc/http
[INCLUDE]
http/httpROMFS.h
[NAME]
httpROMFS
[REDIRECTS]
status /
network /
help /
[SCRIPTS]
api/firmware.php POST firmware_POST_php json 0x1000000
api/test.php GET test_GET_php json 0
api/test.php POST test_POST_php json 8192
api/errors.php GET errors_GET_php json 0
[FILES]
*.js
*.html
*.css
*.map
favicon.ico
api/siteDefaults/*.*
static/media/*.*
*.LICENSE
bsd.txt

```
