/* pagetable.c */
//Zyale Brown-Sanger
//Summer 2025
//Section 1
#include "pagetable.h"
#include "phyframes.h"
#include <stdint.h>

#define NUM_PAGES 32

typedef struct {
    int valid;   // 0 = not resident, 1 = resident
    int frame;   // physical frame number
} page_entry;

static page_entry pagetable[NUM_PAGES];
static unsigned long page_fault_count = 0;

void init_pagetable(void) {
    for (int i = 0; i < NUM_PAGES; i++) {
        pagetable[i].valid = 0;
        pagetable[i].frame = -1;
    }
    page_fault_count = 0;
    init_frames();
}

int translate_page(uint64_t vpage) {
    if (vpage >= NUM_PAGES) return -1;
    if (pagetable[vpage].valid) {
        // Hit
        access_frame(pagetable[vpage].frame);
        return pagetable[vpage].frame;
    }
    // Miss → page fault
    page_fault_count++;
    int evicted;
    int frame = allocate_frame(vpage, &evicted);
    if (evicted >= 0 && evicted < NUM_PAGES) {
        pagetable[evicted].valid = 0;
        pagetable[evicted].frame = -1;
    }
    pagetable[vpage].valid = 1;
    pagetable[vpage].frame = frame;
    return frame;
}

unsigned long get_page_fault_count(void) {
    return page_fault_count;
}
