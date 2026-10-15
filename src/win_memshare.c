#include "memshare.h"

#include <windows.h>

struct MemShareInstance {};

MemShareInstance* create(const char* name, size_t heapSize){
    HANDLE mappedFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        heapSize,
        name
    );

    if (mappedFile == NULL) return NULL;
    
    void* rtn = MapViewOfFile(
        mappedFile,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        heapSize
    );

    if (rtn == NULL) return NULL;

    // allocate memory

    return (MemShareInstance*)rtn; 
}
