/*
 * file.name
 * Description
 * 
 *  Erik Friesen
 *  Copyright (c) 2019
 */

/* 
 * File:   main.c
 * Author: Erik
 *
 * Created on February 3, 2020, 2:07 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include "zlib/zlib.h"

#define VERSION "1.0"
#define MAX_SCRIPT_SIZE 4096
#define MAX_SCRIPT_LINES 512
#define MINIMUM_MALLOC (1024*1024)

char * WEBROOT = NULL;
//char * OBJECT = NULL;
char * OUTPUT_SRC = NULL;
char * OUTPUT_HDR = NULL;
char * INCLUDE = NULL;
char * NAME = NULL;
char * REDIRECTS[MAX_SCRIPT_LINES] = {0};
char * SCRIPTS[MAX_SCRIPT_LINES] = {0};
char * FILES[MAX_SCRIPT_LINES] = {0};
uint32_t lookupTable[MAX_SCRIPT_LINES * 2] = {0};
uint32_t tableLength = 0;
uint32_t fileCount = 0;
uint32_t scriptCount = 0;
uint32_t redirectCount = 0;
char * headerBuf = NULL;
char * sourceBuf = NULL;
char * datfilBuf = NULL;
uint8_t * dataBuf = NULL;
uint32_t headerIdx = 0;
uint32_t sourceIdx = 0;
uint32_t datfilIdx = 0;
uint32_t dataIdx = 0;
uint32_t MAX_FILE_NAME_LENGTH = 0;

void printUsage(void);
size_t fileSize(FILE * f);
int is_regular_file(const char *path);
int is_regular_dir(const char *path);
bool WildCmp(char *pattern, char *string);
int wildcardcmp(const char *pattern, const char *string);
int parseScript(char * script, int len);
int writeHeader(char* data, int len);
int writeSource(char * data, int len);
int writeData(char* data, int len);
uint32_t hash(unsigned char *str, unsigned char *method);

typedef struct {

    union {
        void * Data;
        unsigned char * UBData;
    };
    unsigned int Len;
    unsigned int OriginalLen;
} _ZipObj;

/*
 * 
 */

static voidpf z_alloc_func(voidpf opaque, uInt items, uInt size) {
    return calloc(items, size);
}

static void z_free_func(voidpf opaque, voidpf address) {
    free(address);
}

void gzip_free(_ZipObj * gzip) {
    free(gzip->UBData);
}

int zlib_deflate(unsigned char * datain, int InLen, _ZipObj * gzip) {
    memset(gzip, 0, sizeof (_ZipObj));
    z_stream strm;
    int ret;
    int retval = Z_OK;
    gzip->OriginalLen = InLen;
    gzip->UBData = malloc(InLen);
    if (gzip->UBData == NULL) {
        return Z_MEM_ERROR;
    }
    strm.zalloc = z_alloc_func;
    strm.zfree = z_free_func;
    strm.opaque = Z_NULL;
    ret = deflateInit2(&strm, Z_BEST_COMPRESSION, Z_DEFLATED, 31, 8, Z_DEFAULT_STRATEGY);
    if (ret != Z_OK) {
        return ret;
    }
    strm.avail_in = InLen;
    strm.next_in = datain;
    /* run deflate() on input until output buffer not full, finish
       compression if all of source has been read in */
    strm.avail_out = InLen;
    strm.next_out = gzip->UBData;
    ret = deflate(&strm, Z_FINISH); /* no bad return value */
    gzip->Len = strm.total_out;
    if (strm.avail_in != 0 || ret != Z_STREAM_END) {
        retval = Z_ERRNO;
        gzip->Len = 0;
        free(gzip->UBData);
    }
    gzip->UBData = realloc(gzip->UBData, gzip->Len); //Downsize our allocation
    if (gzip->UBData == NULL) {
        gzip->Len = 0;
        retval = Z_MEM_ERROR;
    }
    /* clean up and return */
    (void) deflateEnd(&strm);
    return retval;
}

void printHeaderStart(void) {
    char * data =
            "/* Processed by yaROMFS version " VERSION " */\r\n"
            "#ifndef YAROMFS_H\r\n"
            "#define YAROMFS_H\r\n\r\n"
    
            "#include <stdint.h>\r\n\r\n"
    
            "#define YAROMFS_GET  1\r\n"
            "#define YAROMFS_POST 2\r\n"
            "#define YAROMFS_HANDLE_INVALID NULL\r\n"
            "\r\n"
            "typedef struct {\r\n"
            "    uint8_t * data;\r\n"
            "    int32_t len;\r\n"
            "    int32_t responseCode;\r\n"
            "    uint8_t gz : 1;\r\n"
            "} _httpResponse;\r\n\r\n"
            "typedef struct {\r\n"
            "    const uint8_t * contentType;\r\n"
            "    uint32_t hash;\r\n"
            "    const uint8_t * data;\r\n"
            "    uint32_t length;\r\n"
            "    _httpResponse (*script_ptr)(uint8_t * data, uint32_t len);\r\n"
            "    uint8_t method;\r\n"
            "    uint8_t gz : 1;\r\n"
            "    uint8_t redirect : 1;\r\n"
            "} _yaROMFSFILE;\r\n\r\n"
            "typedef struct {\r\n"
            "    const _yaROMFSFILE *file;\r\n"
            "    uint32_t position;\r\n"
            "    _httpResponse scriptResponse;\r\n"
            "} YAROMFSFILE_HANDLE;\r\n\r\n"
            "const _yaROMFSFILE * yaROMFSfind(uint8_t *url, uint8_t *method);\r\n"
            "YAROMFSFILE_HANDLE * yaromfs_fopen(uint8_t *url, uint8_t *method, uint8_t *restData, uint32_t length);\r\n"
            "uint32_t yaromfs_fread(YAROMFSFILE_HANDLE *handle, uint8_t *buf, uint32_t len);\r\n"
            "uint32_t yaromfs_f_length(YAROMFSFILE_HANDLE *handle);\r\n"
            "void yaromfs_fclose(YAROMFSFILE_HANDLE *handle);\r\n"
            "const uint8_t * yaromfs_redirect(YAROMFSFILE_HANDLE *handle);\r\n"
            "uint32_t yaromfs_is_gz(YAROMFSFILE_HANDLE *handle);\r\n;"
            "const uint8_t * yaromfs_contentType(YAROMFSFILE_HANDLE *handle);\r\n"
            "uint32_t yaromfs_postExists(uint8_t *url);\r\n"
            "uint32_t yaromfs_responseCode(YAROMFSFILE_HANDLE *handle);\r\n\r\n"
            "/* user defined scripts located in source */\r\n";
    writeHeader(data, strlen(data));
}

void printSourceStart(void) {
    char buf[4096];
    sprintf(buf,
            "/* Processed by yaROMFS version %s */\r\n"
            "/* yaRomfsCore.c must be included  */\r\n"
            "#include <stdint.h>\r\n"
            "#include <ctype.h>\r\n"
            "#include <stddef.h>\r\n"
            "#include <stdlib.h>\r\n"
            "#include \"%s\"\r\n\r\n"
            "#include \"yaROMFSconfig.h\"\r\n\r\n"
            "extern const uint8_t yaROMFSDAT[];\r\n"
            "static const _yaROMFSFILE * lookup(uint8_t * url, uint8_t * method);\r\n\r\n"
            "const _yaROMFSFILE * yaROMFSfind(uint8_t *url, uint8_t *method) {\r\n"
            "    return lookup(url, method);\r\n"
            "}\r\n\r\n"
            "static uint32_t hash(unsigned char *str, unsigned char *method) {\r\n"
            "    uint32_t hash = 5381;\r\n"
            "    int c;\r\n"
            "\r\n"
            "    while (c = *str++) {\r\n"
            "        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */\r\n"
            "    }\r\n"
            "    while (c = *method++) {\r\n"
            "        c = toupper(c);\r\n"
            "        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */\r\n"
            "    }\r\n"
            "    return hash;\r\n"
            "}\r\n\r\n"
            "static const _yaROMFSFILE fileList[] = {\r\n",
            VERSION, INCLUDE
            );
    writeSource(buf, strlen(buf));
}

void printDataFileStart(void) {
    char buf[4096];
    sprintf(buf,
            "/* Processed by yaROMFS version %s */\r\n"
            "#include <stdint.h>\r\n"
            "#include \"yaROMFSconfig.h\"\r\n\r\n",
            VERSION
            );
    writeDataFile(buf, strlen(buf));
}

void printSourceMethods(void) {
    char buf[1024];
    int i;
    sprintf(buf,
            "\r\n"
            "static const _yaROMFSFILE * lookup(uint8_t * url, uint8_t * method){\r\n"
            "	switch (hash(url, method)){\r\n"
            );
    writeSource(buf, strlen(buf));
    for (i = 0; i < tableLength; i++) {
        sprintf(buf,
                "    case 0x%08X:\r\n"
                "        return &fileList[%i];\r\n",
                lookupTable[i], i);
        writeSource(buf, strlen(buf));
    }
    sprintf(buf,
            "    default:\r\n"
            "        return NULL;\r\n"
            "	}\r\n"
            "}\r\n\r\n"
            );
    writeSource(buf, strlen(buf));
}

void processScripts(void) {
    int i;
    uint32_t scriptLen;
    char buf[2048];
    for (i = 0; i < scriptCount; i++) {
        char * scriptName = strtok(SCRIPTS[i], " ");
        char * scriptMethod = strtok(NULL, " ");
        char * scriptSrc = strtok(NULL, " ");
        char * contentType = strtok(NULL, " ");
        char * maxLength = strtok(NULL, " ");
        sprintf(buf, "extern _httpResponse %s(uint8_t * data, uint32_t len);\r\n", scriptSrc);
        writeHeader(buf, strlen(buf));
        uint32_t hashValue = hash(scriptName, scriptMethod);
        lookupTable[tableLength++] = hashValue;
        if (contentType == NULL) {
            contentType = strrchr(scriptName, '.');
            if (contentType == NULL) {
                contentType = "NONE";
            } else {
                contentType++;
            }
        }
        sprintf(buf,
                "    { .hash = 0x%08X, .contentType = \"%s\", .length = %s, .script_ptr = %s, .method = YAROMFS_%s }, /* %s %s */\r\n"
                , hashValue, contentType, maxLength, scriptSrc, scriptMethod, scriptMethod, scriptName);
        writeSource(buf, strlen(buf));
        if (strlen(scriptName) > MAX_FILE_NAME_LENGTH) {
            MAX_FILE_NAME_LENGTH = strlen(scriptName);
        }
    }
}

int32_t processFile(char * fileName, char * url) {
    uint32_t urlLength;
    char buf[1024] = {0};
    FILE * f = fopen(fileName, "rb");
    uint8_t * fileData;
    if (f == NULL) {
        printf("%s not found\r\n", fileName);
        return 1;
    }
    size_t sz = fileSize(f);
    fileData = malloc(sz);
    if (sz != fread(fileData, 1, sz, f)) {
        fclose(f);
        printf("Error reading %s\r\n", fileName);
        return 1;
    }
    fclose(f);
    if (dataIdx % 4) {
        //32 bit align
        writeData("\0\0\0\0", dataIdx % 4);
    }
    _ZipObj gzip;
    if (zlib_deflate(fileData, sz, &gzip)) {
        printf("zlib error\r\n");
        free(fileData);
        return 1;
    }
    uint32_t hashValue = hash(url, "GET");
    lookupTable[tableLength++] = hashValue;
    char * contentType = strrchr(url, '.');
    if (contentType == NULL) {
        contentType = "NONE";
    } else {
        contentType++;
    }
    sprintf(buf,
            "    { .hash = 0x%08X, .contentType = \"%s\", .data = &yaROMFSDAT[0x%X], .length = 0x%X, .gz = 1 },/* %s */\r\n",
            hashValue, contentType, dataIdx, gzip.Len, url);
    writeSource(buf, strlen(buf));
    if (strlen(url) > MAX_FILE_NAME_LENGTH) {
        MAX_FILE_NAME_LENGTH = strlen(url);
    }
    writeData(gzip.UBData, gzip.Len);
    free(fileData);
    gzip_free(&gzip);
    return 0;
}

void searchDirectory(char * dirName, char * wildCard) {
    char buf[1024];
    DIR * searchDir;
    struct dirent *dir;
    //sprintf(buf, "%s/%s", WEBROOT, dirName);
    searchDir = opendir(dirName);
    if (searchDir) {
        while ((dir = readdir(searchDir)) != NULL) {
            sprintf(buf, "%s/%s", dirName, dir->d_name);
            if (is_regular_file(buf)) {
                if (wildcardcmp(wildCard, buf)) {
                    char * url = &buf[strlen(WEBROOT) + 1];
                    printf("URL /%s\n", url);
                    processFile(buf, url);
                }
            } else if (*dir->d_name != '.' && is_regular_dir(buf)) {
                sprintf(buf, "%s/%s", dirName, dir->d_name);
                searchDirectory(buf, wildCard);
            }
        }
        closedir(searchDir);
    }
}

void processWildCard(char * dirName) {
    char buf[1024];
    printf("Searching %s\r\n", dirName);
    if (strstr(dirName, "/")) {
        char * pattern = strrchr(dirName, '/');
        /*int len = strlen(dirName);
        char * pattern = &dirName[len - 1];
        while (*pattern != '/') {
            pattern--;
        }*/
        *pattern = 0;
        pattern++;
        sprintf(buf, "%s/%s", WEBROOT, dirName);
        searchDirectory(buf, pattern);
    } else {
        searchDirectory(WEBROOT, dirName);
    }

}

void processFiles(void) {
    int i;
    char buf[2048];
    for (i = 0; i < fileCount; i++) {
        if (strstr(FILES[i], "*") != NULL) {
            processWildCard(FILES[i]);
        } else {
            sprintf(buf, "%s/%s", WEBROOT, FILES[i]);
            processFile(buf, FILES[i]);
        }
    }
    //Close out for data serialization following
    //writeSource("};\r\n\r\n", strlen("};\r\n\r\n"));
}

void processRedirects(void) {
    char buf[2048];
    uint32_t i;

    for (i = 0; i < redirectCount; i++) {
        char * redirectName = strtok(REDIRECTS[i], " ");
        char * redirectTo = strtok(NULL, " ");
        uint32_t hashValue = hash(redirectName, "GET");
        lookupTable[tableLength++] = hashValue;
        sprintf(buf,
                "    { .hash = 0x%08X, .redirect = 1, .contentType = \"%s\" }, /* Redirect %s -> %s*/\r\n"
                , hashValue, redirectTo, redirectName, redirectTo);
        writeSource(buf, strlen(buf));
        if (strlen(redirectName) > MAX_FILE_NAME_LENGTH) {
            MAX_FILE_NAME_LENGTH = strlen(redirectName);
        }
    }
    writeSource("};\r\n\r\n", strlen("};\r\n\r\n"));
}

void serializeData(void) {
#define ROW_WIDTH 16
    char buf[2048];
    uint32_t i, j, lim, len;
    float size = dataIdx;
    size /= 1024;
    sprintf(buf, "/* yaROMFS size %.1fkb */\r\n", size);
    writeDataFile(buf, strlen(buf));
    sprintf(buf, "const uint8_t _YAROMFSATTR_ yaROMFSDAT[0x%X] = {", dataIdx);
    writeDataFile(buf, strlen(buf));
    for (i = 0; i < dataIdx; i += ROW_WIDTH) {
        len = sprintf(buf, "\r\n    ");
        lim = i + ROW_WIDTH > dataIdx ? dataIdx : i + ROW_WIDTH;
        for (j = i; j < lim; j++) {
            len += sprintf(&buf[len], "0x%02X, ", dataBuf[j]);
        }
        while (j % ROW_WIDTH) {
            len += sprintf(&buf[len], "      ");
            j++;
        }
#ifdef PRINTPRETTY
        len += sprintf(&buf[len], "/* 0x%08X  ", i);
        for (j = i; j < lim; j++) {
            if (dataBuf[j] > ' ' && dataBuf[j] < '~') {
                len += sprintf(&buf[len], "%c", dataBuf[j]);
            } else {
                len += sprintf(&buf[len], ".");
            }
        }
        len += sprintf(&buf[len], " */");
#endif      
        writeDataFile(buf, len);

    }
    sprintf(buf, "\r\n};");
    writeDataFile(buf, strlen(buf));
}

int writeToFile(void) {
    char srcName[512] = {0};
    char hdrName[512] = {0};
    char datName[512] = {0};
    char * old;
    FILE * headerFile = NULL;
    FILE * sourceFile = NULL;
    FILE * datFile = NULL;
    bool writeFlag = false;
    size_t sz;

    // Finalize the file
    sz = sprintf(srcName, "#define MAX_FILE_NAME_LENGTH (%i)\r\n", MAX_FILE_NAME_LENGTH + 16);
    writeHeader(srcName, sz);
    writeHeader("\r\n\r\n#endif\r\n", strlen("\r\n#endif\r\n"));

    strcpy(hdrName, OUTPUT_HDR);
    strcat(hdrName, "/");
    strcat(hdrName, NAME);
    strcat(hdrName, ".h");

    strcpy(srcName, OUTPUT_SRC);
    strcat(srcName, "/");
    strcat(srcName, NAME);
    strcat(srcName, ".c");

    strcpy(datName, OUTPUT_SRC);
    strcat(datName, "/");
    strcat(datName, NAME);
    strcat(datName, "data");
    strcat(datName, ".c");

    headerFile = fopen(hdrName, "rb");
    if (headerFile == NULL) {
        writeFlag = true;
    } else {
        //Compare files
        sz = fileSize(headerFile);
        old = malloc(sz);        
        fread(old, 1, sz, headerFile);
        if (sz != headerIdx) {
            writeFlag = true;
        } else if (memcmp(old, headerBuf, sz)) {
            writeFlag = true;
        }
        free(old);
        fclose(headerFile);
    }
    if (!writeFlag) {
        sourceFile = fopen(srcName, "rb");
        if (sourceFile == NULL) {
            writeFlag = true;
        } else {
            //Compare files
            sz = fileSize(sourceFile);
            old = malloc(sz);
            fread(old, 1, sz, sourceFile);
            if (sz != sourceIdx) {
                writeFlag = true;
            } else if (memcmp(old, sourceBuf, sz)) {
                writeFlag = true;
            }
            free(old);
            fclose(sourceFile);
        }
    }
    if (!writeFlag) {
        datFile = fopen(srcName, "rb");
        if (datFile == NULL) {
            writeFlag = true;
        } else {
            //Compare files
            sz = fileSize(datFile);
            old = malloc(sz);
            fread(old, 1, sz, datFile);
            if (sz != datfilIdx) {
                writeFlag = true;
            } else if (memcmp(old, datfilBuf, sz)) {
                writeFlag = true;
            }
            free(old);
            fclose(datFile);
        }
    }
    if (writeFlag) {
        sourceFile = fopen(srcName, "wb");
        if (sourceFile == NULL) {
            printf("Unable to write file %s\r\n", srcName);
            return 1;
        }
        headerFile = fopen(hdrName, "wb");
        if (headerFile == NULL) {
            fclose(sourceFile);
            printf("Unable to write file %s\r\n", hdrName);
            return 1;
        }
        datFile = fopen(datName, "wb");
        if (headerFile == NULL) {
            fclose(sourceFile);
            fclose(headerFile);
            printf("Unable to write file %s\r\n", datName);
            return 1;
        }
        printf("Writing new files\r\n");
        fwrite(sourceBuf, sourceIdx, 1, sourceFile);
        fclose(sourceFile);
        fwrite(headerBuf, headerIdx, 1, headerFile);
        fclose(headerFile);
        fwrite(datfilBuf, datfilIdx, 1, datFile);
        fclose(datFile);
    } else {
        printf("No changes detected\r\n");
    }
    return 0;
}

int main(int argc, char** argv) {
    FILE * inputScript = NULL;
    size_t result;
    char * buf;
    size_t sz;
    DIR * searchDir;

    printf("yaROMFS file system by Erik Friesen\r\nVERSION %s\r\n", VERSION);
    if (argc < 2) {
        printf("Please specify {1} input config file\r\n");
        printUsage();
        return (EXIT_FAILURE);
    }
    inputScript = fopen(argv[1], "rb");
    if (inputScript == NULL) {
        printf("Unable to open config file \"%s\"\n", argv[1]);
        printUsage();
        return (EXIT_FAILURE);
    }
    sz = fileSize(inputScript);
    buf = calloc(sz, 1);
    result = fread(buf, 1, sz, inputScript);
    fclose(inputScript);
    if (result == 0) {
        printf("Nothing found in file\r\n");
        printUsage();
        return (EXIT_FAILURE);
    }
    if (parseScript(buf, sz)) {
        return (EXIT_FAILURE);
    }
    free(buf);
    printHeaderStart();
    printSourceStart();
    printDataFileStart();
    processScripts();
    processFiles();
    processRedirects();
    printSourceMethods();
    serializeData();
    if (writeToFile()) {
        return (EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}

int parseScript(char * script, int len) {
    char * token;
    char * handler;
    token = strtok(script, "\r\n");
    while (token != NULL) {
        if (strcmp(token, "[WEBROOT]") == 0) {
            handler = "";
            token = strtok(NULL, "\r\n");
            if (token == NULL) {
                break;
            }
            WEBROOT = strdup(token);
        }
        if (strcmp(token, "[OUTPUT_SRC]") == 0) {
            handler = "";
            token = strtok(NULL, "\r\n");
            if (token == NULL) {
                break;
            }
            OUTPUT_SRC = strdup(token);
        }
        if (strcmp(token, "[OUTPUT_HDR]") == 0) {
            handler = "";
            token = strtok(NULL, "\r\n");
            if (token == NULL) {
                break;
            }
            OUTPUT_HDR = strdup(token);
        }
        if (strcmp(token, "[INCLUDE]") == 0) {
            handler = "";
            token = strtok(NULL, "\r\n");
            if (token == NULL) {
                break;
            }
            INCLUDE = strdup(token);
        }
        if (strcmp(token, "[NAME]") == 0) {
            handler = "";
            token = strtok(NULL, "\r\n");
            if (token == NULL) {
                break;
            }
            NAME = strdup(token);
        }
        if (strcmp(token, "[FILES]") == 0) {
            handler = "files";
            token = strtok(NULL, "\r\n");
            continue;
        }
        if (strcmp(token, "[SCRIPTS]") == 0) {
            handler = "scripts";
            token = strtok(NULL, "\r\n");
            continue;
        }
        if (strcmp(token, "[REDIRECTS]") == 0) {
            handler = "redirects";
            token = strtok(NULL, "\r\n");
            continue;
        }
        if (strcmp(handler, "files") == 0) {
            FILES[fileCount] = malloc(strlen(token) + 2);
            FILES[fileCount][0] = '/';
            strcpy(&FILES[fileCount][1], token);
            fileCount++;
            if (fileCount >= MAX_SCRIPT_LINES) {
                printf("File lines exceed %i lines\r\n", MAX_SCRIPT_LINES);
                return (1);
            }
        } else if (strcmp(handler, "scripts") == 0) {
            SCRIPTS[scriptCount] = malloc(strlen(token) + 2);
            SCRIPTS[scriptCount][0] = '/';
            strcpy(&SCRIPTS[scriptCount][1], token);
            scriptCount++;
            if (scriptCount >= MAX_SCRIPT_LINES) {
                printf("Script lines exceed %i lines\r\n", MAX_SCRIPT_LINES);
                return (1);
            }
        } else if (strcmp(handler, "redirects") == 0) {
            REDIRECTS[redirectCount] = malloc(strlen(token) + 2);
            REDIRECTS[redirectCount][0] = '/';
            strcpy(&REDIRECTS[redirectCount][1], token);
            redirectCount++;
            if (scriptCount >= MAX_SCRIPT_LINES) {
                printf("Redirect lines exceed %i lines\r\n", MAX_SCRIPT_LINES);
                return (1);
            }
        }
        token = strtok(NULL, "\r\n");
        if (token >= script + len) {
            break;
        }
    }

    if (WEBROOT == NULL) {
        printf("Missing [WEBROOT] in script file\r\n");
        return (1);
    }
    if (OUTPUT_SRC == NULL) {
        printf("Missing [OUTPUT_SRC] in script file\r\n");
        return (1);
    }
    if (OUTPUT_HDR == NULL) {
        printf("Missing [OUTPUT_HDR] in script file\r\n");
        return (1);
    }
    if (INCLUDE == NULL) {
        printf("Missing [INCLUDE] in script file\r\n");
        return (1);
    }
    if (NAME == NULL) {
        printf("Missing [NAME] in script file\r\n");
        return (1);
    }
    return 0;
}

int writeHeader(char* data, int len) {
    if (headerBuf == NULL) {
        headerBuf = malloc(len);
    } else {
        headerBuf = realloc(headerBuf, headerIdx + len);
    }
    memcpy(&headerBuf[headerIdx], data, len);
    headerIdx += len;
}

int writeSource(char * data, int len) {
    static uint32_t mallocLength = 0;
    if (sourceBuf == NULL) {
        sourceBuf = malloc(MINIMUM_MALLOC);
        mallocLength = MINIMUM_MALLOC;
    } else {
        while (sourceIdx + len >= mallocLength) {
            sourceBuf = realloc(sourceBuf, mallocLength + MINIMUM_MALLOC);
            mallocLength += MINIMUM_MALLOC;
        }        
    }
    memcpy(&sourceBuf[sourceIdx], data, len);
    sourceIdx += len;
}

int writeDataFile(char * data, int len) {
    static uint32_t mallocLength = 0;
    if (datfilBuf == NULL) {
        datfilBuf = malloc(MINIMUM_MALLOC);
        mallocLength = MINIMUM_MALLOC;
    } else {
        while (datfilIdx + len >= mallocLength) {
            datfilBuf = realloc(datfilBuf, mallocLength + MINIMUM_MALLOC);
            mallocLength += MINIMUM_MALLOC;
        }        
    }
    memcpy(&datfilBuf[datfilIdx], data, len);
    datfilIdx += len;
}

int writeData(char* data, int len) {
    static uint32_t mallocLength = 0;
    if (dataBuf == NULL) {
        dataBuf = malloc(MINIMUM_MALLOC);
        mallocLength = MINIMUM_MALLOC;
    } else {
        while (dataIdx + len >= mallocLength) {
            dataBuf = realloc(dataBuf, mallocLength + MINIMUM_MALLOC);
            mallocLength += MINIMUM_MALLOC;
        }
    }
    memcpy(&dataBuf[dataIdx], data, len);
    dataIdx += len;
}

size_t fileSize(FILE * f) {
    fseek(f, 0L, SEEK_END);
    size_t sz = ftell(f);
    rewind(f);
    return sz;
}

int is_regular_file(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

int is_regular_dir(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

bool WildCmp(char *pattern, char *string) {
    static int levels = 0;
    printf("Level %i:%s:%s\r\n", levels, pattern, string);
    levels++;
    if (*pattern == '\0' && *string == '\0') // Check if string is at end or not.
        return true;

    if (*pattern == '?' || *pattern == *string) //Check for single character missing or match
        return WildCmp(pattern + 1, string + 1);

    if (*pattern == '*')
        return WildCmp(pattern + 1, string) || WildCmp(pattern, string + 1); // Check for multiple character missing
    levels--;
    return false;
}

int wildcardcmp(const char *pattern, const char *string) {
    const char *w = NULL; // last `*`
    const char *s = NULL; // last checked char
    bool matched = false;
    // malformed
    if (!pattern || !string) return 0;
    /*if (strstr(string, "/")) {
        while(*string++ != '/');
    }
    if (strstr(string, "\\")) {
        while (*string++ != '\\');
    }*/
    // loop 1 char at a time
    while (1) {
        if (!*string) {
            if (!*pattern && matched) return 1;
            if (!*s) return 0;
            string = s++;
            pattern = w;
            continue;
        } else {
            if (*pattern != *string) {
                if ('*' == *pattern) {
                    w = ++pattern;
                    s = string;
                    // "*" -> "foobar"
                    if (*pattern) continue;
                    return 1;
                } else if (w) {
                    matched = false;
                    string++;
                    // "*ooba*" -> "foobar"
                    continue;
                }
                return 0;
            }
        }
        matched = true;
        string++;
        pattern++;
    }

    return 1;
}

uint32_t hash(unsigned char *str, unsigned char *method) {
    uint32_t hash = 5381;
    int c;

    while (c = *str++) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    while (c = *method++) {
        c = toupper(c);
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

void printUsage(void) {
    printf(
            "yaROMFS configfile.txt\r\n"
            "Example config:\r\n"
            "[WEBROOT]"
            "C:/web\r\n"
            "[OUTPUT_SRC]\r\n"
            "C:/src\r\n"
            "[OUTPUT_HDR]\r\n"
            "C:/inc\r\n"
            "[INCLUDE]\r\n"
            "httpROMFS.h"
            "[NAME]"
            "httpROMFS"
            "[SCRIPTS]"
            "api/emsetup.php GET emsetup_GET_PHP\r\n"
            "[FILES]\r\n"
            "*.js\r\n"
            "somefolder/*.*\r\n"
            );
}

