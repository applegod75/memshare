#include <stdlib.h>
#include <stdint.h>

// a slightly modified implementation of the fysos memory allocation system:
// https://www.fysnet.net/bucket/specification.php

#define PLF_USED 0x1
#define PLF_ALIGNED 0x2
#define PLF_CLEARED 0x4
#define PLF_NOT_OPERATIVE 0x8000

#define BLF_BEST_FIT 0x01

#pragma pack(push, 1)

struct Bucket {
    uint32_t magic;
    uint32_t flags;
    size_t largest;
    size_t size;
    uint32_t lock;
    void* firstPebble;
};

struct Pebble {
    uint32_t magic;
    uint32_t flags;
    uint32_t alignment;
    size_t size;
    char name[32];
    void* parent;
    void* prev;
    void* next;
};

#pragma pack(pop)

inline struct Bucket createBucket(){
    struct Bucket buck;
    buck.magic = 0x4255434B;
    return buck;
}

inline struct Pebble createPebble(){
    struct Pebble peb;
    peb.magic = 0x524F434B;
    return peb;
}

void* ms_internal_writeBucket(void* mem, size_t maxSize);
void* ms_internal_mergePebbles(void* pb1, void* pb2);
