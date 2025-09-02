// assign3part1.c
// Zyale Brown-Sanger
// Saummer 2025
// Section 1 
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

#define PAGE_SIZE   128   // 128 bytes per page
#define NUM_PAGES   32    // 4096 bytes virtual space / 128 bytes
#define NUM_FRAMES  8     // 1024 bytes physical memory / 128 bytes

// Fixed page-to-frame mapping that follows Figure 1 that is in the assignment pdf 
const int page_to_frame[NUM_PAGES] = {
    /* 0 */ 2,
    /* 1 */ 4,
    /* 2 */ 1,
    /* 3 */ 7,
    /* 4 */ 3,
    /* 5 */ 5,
    /* 6 */ 6,
    /* 7..14 */ -1,-1,-1,-1,-1,-1,-1,-1,
    /* 15..22 */ -1,-1,-1,-1,-1,-1,-1,-1,
    /* 23..30 */ -1,-1,-1,-1,-1,-1,-1,-1,
    /* 31 */ -1
};

static size_t alignup(size_t size, size_t align) {
    return (size + align - 1) & ~(align - 1);
}
//main method
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input sequence filename>\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char *seqfile = argv[1];

    int fd = open(seqfile, O_RDONLY);
    // error handling for opening the file 
    if (fd < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("fstat");
        close(fd);
        return EXIT_FAILURE;
    }
    size_t filesize = st.st_size;
    size_t mapsize  = alignup(filesize, PAGE_SIZE);

    uint64_t *memAccesses = mmap(NULL, mapsize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (memAccesses == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return EXIT_FAILURE;
    }
    close(fd);
    
    // Outputs the translated physical addresses to a file
    FILE *out = fopen("output-part1", "wb");
    if (!out) {
        perror("fopen output-part1");
        munmap(memAccesses, mapsize);
        return EXIT_FAILURE;
    }

    size_t count = filesize / sizeof(uint64_t);
    for (size_t i = 0; i < count; i++) {
        uint64_t vaddr = memAccesses[i];
        uint64_t page  = vaddr >> 7;        // vaddr / 128
        uint64_t offset = vaddr & 0x7F;     // vaddr % 128
	
	// error handling for virtual page
        if (page >= NUM_PAGES) {
            fprintf(stderr, "Bad virtual page %lu for vaddr 0x%" PRIx64 "\n", page, vaddr);
            continue;
        }
	
	// error handling for mapping to page 
        int frame = page_to_frame[page];
        if (frame < 0 || frame >= NUM_FRAMES) {
            fprintf(stderr, "No mapping for virtual page %lu\n", page);
            continue;
        }

        uint64_t paddr = ((uint64_t)frame << 7) | offset;
        if (fwrite(&paddr, sizeof(paddr), 1, out) != 1) {
            perror("fwrite");
            break;
        }
	// print statement that formats how it is laid out on screen 
        printf("logical address is 0x%" PRIx64 ", physical address is 0x%" PRIx64 "\n", vaddr, paddr);
    }

    fclose(out);
    munmap(memAccesses, mapsize);
    return EXIT_SUCCESS;
}
