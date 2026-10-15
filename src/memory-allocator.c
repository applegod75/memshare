#include "memory-allocator.h"
#include <string.h>

void* ms_internal_writeBucket(void* mem, size_t size){
    // fit a bucket, pebble, and the minimum space for a pebble (64 bytes)
    if (size < (sizeof(struct Bucket) + sizeof(struct Pebble) + 64)){
        return NULL;
    }

    struct Bucket buck = createBucket();
    buck.size = size - sizeof(struct Bucket);
    // cast to a byte* because void* arithmetic is disallowed.
    buck.firstPebble = (uint8_t*)mem + sizeof(struct Bucket);
     // no default flags.
    buck.flags = 0;
    // initialize the entire bucket to 64-byte pebbles
    buck.largest = 64;
    // start the bucket unlocked.
    buck.lock = 0;
    
    memcpy(mem, &buck, sizeof(struct Bucket));
    
    size_t pebbleAmount = (size - sizeof(struct Bucket)) / (sizeof(struct Pebble) + 64);
    size_t minPebbleSize = sizeof(struct Pebble) + 64;
    for (size_t i = 0; i < pebbleAmount; i++){
        uint64_t pebbleOffset = minPebbleSize * i;
        struct Pebble peb = (struct Pebble) {
            .magic = 0x524F434B,
            .flags = PLF_ALIGNED | PLF_CLEARED,
            .size = 64,
            .alignment = 64,
            .name = "",
            .next = (i + 1 != pebbleAmount) ? ((uint8_t*)mem + (pebbleOffset + minPebbleSize)) : (NULL),
            .prev = (i != 0) ? ((uint8_t*)mem + (pebbleOffset - minPebbleSize)) : (NULL),
            .parent = mem
        };

        memcpy((uint8_t*)mem + pebbleOffset, &peb, sizeof(struct Pebble));
        memset((uint8_t*)mem + pebbleOffset + sizeof(struct Pebble), 0, 64);
    }
    return mem;
}

void* ms_internal_mergePebbles(void* peb1, void* peb2){
    if (*(uint64_t*)peb1 != 0x524F434B || *(uint64_t*)peb2 != 0x524F434B){
        return NULL;
    }
        
    struct Pebble* pb1 = (struct Pebble*)peb1;
    struct Pebble* pb2 = (struct Pebble*)peb2;
    if (pb1->next != peb2 || pb2->prev != peb1){
        return NULL;
    }

    void* prev = pb1->prev;
    void* next = pb2->next;
    
    uint32_t flags = 0;
    if (pb1->flags & PLF_ALIGNED && pb2->flags & PLF_ALIGNED){
        flags |= PLF_ALIGNED;
    }
    if (pb1->alignment != pb2->alignment){
        flags &= (~PLF_ALIGNED);
    }
    flags |= PLF_CLEARED;

    struct Pebble peb = (struct Pebble) {
        .magic = 0x524F434B,
        .flags = flags,
        // also delete the second pebble's header.
        .size = pb1->size + pb2->size + sizeof(struct Pebble),
        // writing the new header to pb1's location
        .alignment = pb1->alignment,
        .prev = prev,
        .next = next,
        .name = "",
        // if the parents aren't the same, the user is intentionally fucking with me
        .parent = pb1->parent
    };

    memcpy(peb1, &peb, sizeof(struct Pebble));
    memset((uint8_t*)peb1 + sizeof(struct Pebble), 0, peb.size);
    return peb1;
}
