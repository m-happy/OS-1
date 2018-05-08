#include "libmymem.hpp"
#include <iostream>
#include <sys/mman.h>
#include <mutex>
#define DEB // For debugging purpose.
/**
 * An array of all buckets.
 * Each stores object of size 2^(index+2)
 */

// static to prevent external linkage. :: Private to this file only.
static bucket_t hashTable[] = { /* 12 Elements */
	bucket_t{4, 5363, nullptr},
	bucket_t{8, 4022, nullptr},
	bucket_t{16, 2681,nullptr},
	bucket_t{32, 1609,nullptr},
	bucket_t{64, 893, nullptr},
	bucket_t{128, 473, nullptr},
	bucket_t{256, 243, nullptr},
	bucket_t{512, 123, nullptr},
	bucket_t{1024, 62, nullptr},
	bucket_t{1<<11, 31, nullptr},
	bucket_t{1<<12, 15, nullptr},
	bucket_t{1<<13, 7, nullptr},
};

static std::mutex locks[12];

// static to prevent external linkage. :: Private to this file only.
static int bestFit(unsigned size){
	if(size<=4) return 0;
	else if(size <=8) return 1;
	else if(size <=16) return 2;
	else if(size <=32) return 3;
	else if(size <=64) return 4;
	else if(size <=128) return 5;
	else if(size <=256) return 6;
	else if(size <=512) return 7;
	else if(size <=1024) return 8;
	else if(size <=2048) return 9;
	else if(size <=4096) return 10;
	else if(size <=8192) return 11;
	else return -1;
} // best fit

void *mymalloc(unsigned size){
	int index = bestFit(size);
	
	if(index<0) {
		#ifdef DEB
			std::cerr<<"Index < 0 bestFit"<<std::endl;
		#endif
		return nullptr;
	}

	// lock corresponding mutex!
	locks[index].lock();

	if(hashTable[index].firstSlab==nullptr){ // First slab of the size.
		slab_t *slbPtr = (slab_t *) mmap(NULL, 1<<16, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		hashTable[index].firstSlab = slbPtr;
		// Initialize slab variables.
		slbPtr->thisBucket = &(hashTable[index]);
		slbPtr->totObj = (slbPtr->thisBucket)->totObj;
		slbPtr->freeObj = slbPtr->totObj;
		slbPtr->nextSlab = nullptr;
		slbPtr->bitmap.reset();
		// Give first object.
		// var is the first free object's location.
		slbPtr->freeObj--;
		slbPtr->bitmap[0]=1;
		// Need to jump by 1 byte * sizeof(slab_t)
		// Need to store sizeof(pointer) in this location.
		slab_t **p2 = (slab_t **)(slbPtr+1);
		#ifdef DEB
			std::cerr<<"slbPtr: "<<slbPtr<<", p2: "<<p2<<", p2+1: "<<p2+1<<std::endl;
		#endif
		*p2 = slbPtr;
		locks[index].unlock();
		return (void *)(p2+1);
	} // No slab exists
	else{ // goto the next non empty slab.
		slab_t *slbPtr = hashTable[index].firstSlab;
		while(slbPtr->nextSlab!=nullptr && (slbPtr->freeObj <= 0)) slbPtr = slbPtr->nextSlab;
		if(slbPtr->freeObj <=0 ){ // Cause of loop break was all slabs full.
			slab_t *newSlbPtr = (slab_t *) mmap(NULL, 1<<16, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
			slbPtr->nextSlab = newSlbPtr;
			// Initialize slab variables.
			newSlbPtr->thisBucket = &(hashTable[index]);
			newSlbPtr->totObj = (slbPtr->thisBucket)->totObj;
			newSlbPtr->freeObj = slbPtr->totObj;
			newSlbPtr->nextSlab = nullptr;
			newSlbPtr->bitmap.reset();
			// Give first object.
			// var is the first free object's location.
			newSlbPtr->freeObj--;
			newSlbPtr->bitmap[0]=1;
			// Need to jump by 1 byte * sizeof(slab_t)
			// Need to store sizeof(pointer) in this location.
			slab_t **p2 = (slab_t **)(newSlbPtr+1);
			#ifdef DEB
				std::cerr<<"All slabs full: insert:: newSlbPtr: "<<newSlbPtr<<", p2: "<<p2<<", p2+1: "<<p2+1<<std::endl;
			#endif
			*p2 = newSlbPtr;
			locks[index].unlock();
			return (void *)(p2+1);
		} // All slabs full.
		else{ // Slab exists and is partially full.
			// find first non empty object place.
			unsigned i {0};
			for(; slbPtr->bitmap[i]!=0 && (i < slbPtr->totObj); i++); // Loop till you get to a 0.
			bool *p3 = (bool *)(slbPtr+1); // bool is 1 byte.
			p3+=(i * (((slbPtr->thisBucket)->objSize)+sizeof(bool *))); // Get to the current pos. 
			//this should be sizeof(struct object_t)
			slbPtr->freeObj--;
			slbPtr->bitmap[i]=1;
			slab_t **p2 = (slab_t **) p3;
			#ifdef DEB
				std::cerr<<"Partially full slab: insert:: SlbPtr: "<<slbPtr<<", p2: "<<p2<<", p2+1: "<<p2+1<<std::endl;
			#endif
			*p2 = slbPtr;
			locks[index].unlock();
			return (void *)(p2+1); 
		} // else of slab exists, is partially full.
	} // else of no slab exists yet
} // myalloc

void myfree(void *ptr){
	for(int i=0;i<12;i++) locks[i].lock();
	if(ptr==nullptr){
		#ifdef DEB
			std::cerr<<"Freeing nullptr error"<<std::endl;
		#endif
	}
	slab_t * slbPtr = *( ((slab_t **)ptr)-1); // The slab that contains this object.
	slab_t ** ptr2slbPtr = ((slab_t **)ptr)-1;
	slab_t **p2 = (slab_t **)(slbPtr+1); // pointer to first object.
	unsigned offset = ((bool*)ptr2slbPtr - (bool*)p2)/(((slbPtr->thisBucket)->objSize)+sizeof(bool *));
	// all ptrs have same size.
	if(slbPtr->bitmap[offset]!=1){
		#ifdef DEB
			std::cerr<<">>>>>>> Multiple free(s). ERROR."<<std::endl;
		#endif
		for(int i=0;i<12;i++) locks[i].unlock();

		return;
	}
	if(slbPtr->bitmap.count() > 1){ // many objects left, dont delete this
		slbPtr->bitmap[offset]=0;
		slbPtr->freeObj++;
		#ifdef DEB
			std::cerr<<"Deletion Partially full slab: SlbPtr: "<<slbPtr<<", p2: "<<p2<<", offset: "<<offset<<std::endl;
		#endif
		for(int i=0;i<12;i++) locks[i].unlock();

		return;
	}
	else{ // only 1 object in this slab, need to unmap.
		slab_t * iterator = (slbPtr->thisBucket)->firstSlab;
		if(iterator == nullptr){ // sanity check, will never happen.
			#ifdef DEB
			std::cerr<<"Deletion of nullptr, SEGFAULT: SlbPtr: "<<slbPtr<<", p2: "<<p2<<", offset: "<<offset<<std::endl;
			#endif
			for(int i=0;i<12;i++) locks[i].unlock();
			return;
		}
		else if(iterator == slbPtr){ // Need to delete head.
			slab_t * nxt = slbPtr->nextSlab;
			(slbPtr->thisBucket)->firstSlab = nxt;
			munmap(slbPtr, 1<<16);
			#ifdef DEB
				std::cerr<<"Deletion (firstSlab) empty slab: SlbPtr: "<<slbPtr<<", p2: "<<p2<<", offset: "<<offset<<std::endl;
			#endif
			for(int i=0;i<12;i++) locks[i].unlock();

			return;
		}
		else{ // Slab to be removed is somewhere in b/w
			slab_t * prev = iterator;
			for(; prev!=nullptr && slbPtr != prev->nextSlab; prev=prev->nextSlab);
			prev->nextSlab = slbPtr->nextSlab;
			munmap(slbPtr, 1<<16);
			#ifdef DEB
				std::cerr<<"Deletion empty slab: SlbPtr: "<<slbPtr<<", p2: "<<p2<<", offset: "<<offset<<std::endl;
			#endif
			for(int i=0;i<12;i++) locks[i].unlock();

			return;
		}
	} // else only 1 object in slab.
} // myfree
