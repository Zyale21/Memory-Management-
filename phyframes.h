/* phyframes.h */
//Zyale Brown-Sanger
//Summer 2925
//Section 1 

#include <stdint.h>

// Initialize physical‐frame table (frame 0 reserved for OS)
void init_frames(void);

// Allocate a frame for virtual page `vpage`.  
// On eviction, *evicted_page = old_vpage (or -1 if none).  
// Returns the frame index [0..7].
int allocate_frame(uint64_t vpage, int *evicted_page);
// Update LRU timestamp for a hit on `frame`
void access_frame(int frame);


