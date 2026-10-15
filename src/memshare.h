
#pragma once
#include <stdint.h>

typedef struct MemShareInstance MemShareInstance;

/**
 * creates a new memory sharing instance by a specific name.
 *
 * The heapSize is the size of the heap for that section of memory.
 * Because of some restraints, and to make the library simpler, this
 * needs to all be preallocated.
 */
MemShareInstance* ms_create(const char* name, size_t heapSize);
/**
 * connects to an existing memory sharing instance by a specific name.
 */
MemShareInstance* ms_connect(const char* name);

/**
 * allocates a new block of memory in whatever instance you pass in.
 *
 * The memory block will be of size `size` and will be given a name
 * according to the parameter `name` which other processes can later
 * use to read from the memory.
 */
void* ms_alloc(MemShareInstance* instance, size_t size, const char* name);

/**
 * Enumerates all the memory blocks available by their given names.
 *
 * Memory is managed by the function
 */
const char** m_enumerateNames(MemShareInstance* instance, size_t* len);

/**
 * reads memory based on the `name` parameter. This call *DOES NOT* wait if the
 * memory being accessed is locked by a mutex.
 */
void* ms_read(
    MemShareInstance* instance,
    const char* name,
    size_t* size
);


/**
 * writes some memory to whatever section you specify at a specific
 * offset.
 *
 * This function also *DOES NOT* wait if the memory being accessed is locked.
 */
void ms_write(
    MemShareInstance* instance,
    const char* name,
    size_t offset,
    void* data,
    size_t size
);

/**
 * Locks the mutex on a section of memory you specify.
 *
 * If the section is already locked, this function waits until
 * this section gets unlocked
 */
void ms_lockSection(MemShareInstance* instance, const char* name);
/**
 * Simply waits for a section's mutex to be unlocked.
 *
 * If the section's mutex is already unlocked, this does nothing.
 */
void ms_waitForUnlock(MemShareInstance* instance, const char* name);
/**
 * Simply unlocks the mutex on a section.
 */
void ms_unlockSection(MemShareInstance* instance, const char* name);
