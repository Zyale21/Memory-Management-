/* phyframes.c */
//Zyale Brown-Sanger
//Summer 2025
//Section 1
#include "phyframes.h"
#include <stdint.h>

#define NUM_FRAMES 8

// frame_to_page[f] = resident virtual page, or:
//   -2 if reserved for OS, -1 if free
static int  frame_to_page[NUM_FRAMES];
static unsigned long frame_last_used[NUM_FRAMES];
static unsigned long current_time = 0;

void init_frames(void) {
    frame_to_page[0]    = -2;   // OS reserves frame 0
    frame_last_used[0]  = 0;
    for (int f = 1; f < NUM_FRAMES; f++) {
        frame_to_page[f]   = -1; // free
        frame_last_used[f] = 0;
    }
}

int allocate_frame(uint64_t vpage, int *evicted_page) {
    *evicted_page = -1;
    // 1) Try to find a free frame (1..7)
    for (int f = 1; f < NUM_FRAMES; f++) {
        if (frame_to_page[f] == -1) {
            frame_to_page[f]    = (int)vpage;
            frame_last_used[f]  = ++current_time;
            return f;
        }
    }
    // 2) Otherwise, LRU eviction among 1..7
    int lru = 1;
    for (int f = 2; f < NUM_FRAMES; f++) {
        if (frame_last_used[f] < frame_last_used[lru])
            lru = f;
    }
    *evicted_page       = frame_to_page[lru];
    frame_to_page[lru]  = (int)vpage;
    frame_last_used[lru] = ++current_time;
    return lru;
}

void access_frame(int frame) {
    if (frame > 0 && frame < NUM_FRAMES) {
        frame_last_used[frame] = ++current_time;
    }
}

