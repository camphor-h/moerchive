#ifndef MOERCHIVE__
#define MOERCHIVE__

#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#endif

#define SIGNATURE_LENGTH 4
#define DEFAULT_BUFFER_SIZE 1024

extern const char mar_signature[SIGNATURE_LENGTH];

/*The definition of struct is only for describe the structure of .mar file*/
/*In fact, I didn't use it in code*/

typedef struct Header
{
    int files_amount;
}Header;
typedef struct File_chunk
{
    uint8_t file_name_length;
    char* file_name;
    int64_t file_size;
    char* data;
}File_chunk;

typedef char BOOL;
#define TRUE 1
#define FALSE 0

#endif