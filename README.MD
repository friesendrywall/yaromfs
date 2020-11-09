## yaromfs

This the source for building the read only file system for the
EasyMix web-server, and probably others in the future.  This is
intended to be incorporated with a modified FreeRTOS webserver.

## Build

Windows build using MINGW.

## Usage

This is built using input files rather than params.
An example usage would be:

```myromfs web-source.txt```

web-source.txt example

```
[WEBROOT]
C:/Projects/EasyMix/EM12/web/build
[OUTPUT_SRC]
C:/Projects/EasyMix/EM12/firmware/src/http
[OUTPUT_HDR]
C:/Projects/EasyMix/EM12/firmware/inc/http
[INCLUDE]
http/httpROMFS.h
[NAME]
httpROMFS
[REDIRECTS]
status /
network /
EMSetup /
siteSetup /
utilities /
update /
help /
[SCRIPTS]
api/firmware.php POST firmware_POST_php json 0x1000000
api/emsetup.php GET emsetup_GET_php json 0
api/emsetup.php POST emsetup_POST_php json 8192
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