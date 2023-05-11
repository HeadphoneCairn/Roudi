// MemoryFree library based on code posted here:
// https://forum.arduino.cc/index.php?topic=27536.msg204024#msg204024
// Extended by Matthew Murdoch to include walking of the free list.
// Extended by Headphone Cairn Software with fragmentation count.

#ifndef MEMORY_FREE_H
#define MEMORY_FREE_H

#ifdef __cplusplus
extern "C" {
#endif

int getFreeMemory();
int getNumberOfHeapFragments();
int getMallocMargin();

#ifdef  __cplusplus
}
#endif

#endif