/* assign3part3.c: Generalized Page Table Simulator with Binary Output */
//Zyale Brown-Sanger
//Summer 2025
//Section 1
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <PageSize> <VirtMemSize> <PhysMemSize> <SequenceFile>\n", argv[0]);
        return EXIT_FAILURE;
    }
    // Parse parameters
    size_t pageSize = strtoul(argv[1], NULL, 0);
    size_t virtSize = strtoul(argv[2], NULL, 0);
    size_t physSize = strtoul(argv[3], NULL, 0);
    const char *seqfile = argv[4];
    if (!pageSize || !virtSize || !physSize) {
        fprintf(stderr, "Invalid sizes\n");
        return EXIT_FAILURE;
    }
    if (virtSize % pageSize || physSize % pageSize) {
        fprintf(stderr, "Sizes must be multiples of page size\n");
        return EXIT_FAILURE;
    }
    size_t numPages  = virtSize / pageSize;
    size_t numFrames = physSize / pageSize;

    // Allocate page table and frame data
    typedef struct { int valid; size_t frame; } PageEntry;
    PageEntry *pageTable = calloc(numPages, sizeof(PageEntry));
    int *frameToPage     = malloc(numFrames * sizeof(int));
    unsigned long *lastUsed = calloc(numFrames, sizeof(unsigned long));
    if (!pageTable || !frameToPage || !lastUsed) {
        perror("Allocation");
        return EXIT_FAILURE;
    }
    for (size_t f = 0; f < numFrames; f++) frameToPage[f] = -1;

    unsigned long currentTime = 0;
    unsigned long pageFaults  = 0;

    // mmap sequence file
    int fd = open(seqfile, O_RDONLY);
    if (fd < 0) { perror("open"); return EXIT_FAILURE; }
    struct stat st;
    if (fstat(fd, &st) < 0) { perror("fstat"); close(fd); return EXIT_FAILURE; }
    size_t count = st.st_size / sizeof(uint64_t);
    size_t mapsize = (st.st_size + pageSize - 1) & ~(pageSize - 1);
    uint64_t *vaddrs = mmap(NULL, mapsize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (vaddrs == MAP_FAILED) { perror("mmap"); close(fd); return EXIT_FAILURE; }
    close(fd);

    // Open binary output
    FILE *out = fopen("output-part3", "wb");
    if (!out) { perror("fopen"); munmap(vaddrs, mapsize); return EXIT_FAILURE; }

    // Process each address
    for (size_t i = 0; i < count; i++) {
        uint64_t vaddr = vaddrs[i];
        size_t vpage  = vaddr / pageSize;
        size_t offset = vaddr % pageSize;
        if (vpage >= numPages) {
            fprintf(stderr, "Invalid page %zu\n", vpage);
            continue;
        }
        size_t frame;
        if (pageTable[vpage].valid) {
            frame = pageTable[vpage].frame;
            lastUsed[frame] = ++currentTime;
        } else {
            // page fault
            pageFaults++;
            // find free frame
            ssize_t f;
            for (f = 0; f < (ssize_t)numFrames; f++) {
                if (frameToPage[f] < 0) break;
            }
            if (f == (ssize_t)numFrames) {
                // LRU eviction
                f = 0;
                for (ssize_t j = 1; j < (ssize_t)numFrames; j++) {
                    if (lastUsed[j] < lastUsed[f]) f = j;
                }
                // invalidate old page
                pageTable[frameToPage[f]].valid = 0;
            }
            frameToPage[f] = (int)vpage;
            pageTable[vpage].valid = 1;
            pageTable[vpage].frame = f;
            lastUsed[f] = ++currentTime;
            frame = f;
        }
        uint64_t paddr = ((uint64_t)frame * pageSize) + offset;
        // print console
        printf("Logical Address = 0x%" PRIx64 " :: pageEntry = %zu     Frame Number = %zu :: Physical address: 0x%" PRIx64 "\n",
               vaddr, vpage, frame, paddr);
        // write binary
        if (fwrite(&paddr, sizeof(paddr), 1, out) != 1) {
            perror("fwrite"); break;
        }
    }

    // summary
    printf("Page fault count = %lu\n", pageFaults);

    fclose(out);
    munmap(vaddrs, mapsize);
    free(pageTable);
    free(frameToPage);
    free(lastUsed);
    return EXIT_SUCCESS;
}

