/* pagetable.h */
//Zyale Brown-Sanger
//Summer 2025
//Section 1

#include <stdint.h>

// Initialize your page table & physical frames
void init_pagetable(void);

// Map virtual page `vpage` → physical frame (LRU‐allocate on miss).
// Returns frame idx [0..7], or -1 on invalid vpage.
int translate_page(uint64_t vpage);

// gets the page_fault count needed for part 2 and 3 of the assignment. 
unsigned long get_page_fault_count(void);


