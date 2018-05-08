#ifndef _LIB_MYMEM_HPP
#define _LIB_MYMEM_HPP

#include <bitset>

/**
 * OS slab allocator assignment.
 */

/**
 * The function to be exported in the API.
 * @param size How many bytes to allocate? [description]
 * @return      Pointer to the allocated memory.
 */
void *mymalloc(unsigned size);
void myfree(void *ptr);

struct slab_t{
	struct bucket_t *thisBucket;
	unsigned totObj;
	unsigned freeObj;
	slab_t * nextSlab;
	std::bitset<1<<13> bitmap; // Fixed size bitmap. 	
};


struct object_t{
	slab_t * slab;
};


struct bucket_t{
	unsigned objSize;
	unsigned totObj;
	slab_t * firstSlab;
};



#endif /* _LIB_MYMEM_HPP */