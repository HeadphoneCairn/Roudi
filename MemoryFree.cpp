/*
Check out
  https://www.nongnu.org/avr-libc/user-manual/malloc.html
for more information about memory management.
*/

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

extern unsigned int __heap_start;
extern unsigned int __malloc_margin;
extern void *__brkval;

/*
 * The free list structure as maintained by the
 * avr-libc memory allocation routines.
 */
struct __freelist {
  size_t sz;
  struct __freelist *nx;
};

/* The head of the free list structure */
extern struct __freelist *__flp;

#include "MemoryFree.h"

/* Calculates the size of the free list */
static int freeListSize() {
  struct __freelist* current;
  int total = 0;
  for (current = __flp; current; current = current->nx) {
    total += 2; /* Add two bytes for the memory block's header  */
    total += (int) current->sz;
  }
  return total;
}

/*
  Gets the total amount of free memory (in bytes). This is the space 
  between the heap and the stack (they grow towards each other) + 
  all the bytes in the empty blocks inside the heap.

  The value returned includes the __malloc_margin!
  See getMallocMargin().
*/
int getFreeMemory() {
  int free_memory;
  if ((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__heap_start);
  } else {
    free_memory = ((int)&free_memory) - ((int)__brkval);
    free_memory += freeListSize();
  }
  return free_memory;
}

/* 
  Returns the number of fragments of which the heap is made of.
  - If the result is 1, there is no fragmentation. 
    All heap memory is available as one contiguous block of memory.
  - If the result is > 1, the heap is fragmented. This should be
    prevented as much as possible.
*/
int getNumberOfHeapFragments() {
  struct __freelist* current;
  int size_of_free_list = 1;
  for (current = __flp; current; current = current->nx) {
    size_of_free_list++;
  }
  return size_of_free_list; 
}

/*
  Returns the __malloc_margin, which is the amount of bytes that the 
  HEAP must stay away from the current stack.
  When malloc is called and the request would cause the space between
  the heap and the current stack to be < __malloc_margin, the malloc
  fails. 
  By default, the margin is 128 bytes.
  You can set the __malloc_margin at startup if you want to change it.
*/
int getMallocMargin()
  {
  return (int)__malloc_margin; 
  }
