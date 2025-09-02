/* assign3part2.c */
//Zyale Brown-Sanger
//Sunmer 2025
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
#include "pagetable.h"

#define PAGE_SIZE 128

// main method
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input sequence filename>\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char *seqfile = argv[1];

    //error handling for opening the file 
    int fd = open(seqfile, O_RDONLY);
    if (fd < 0) { perror("open"); return EXIT_FAILURE; }
    struct stat st;
    if (fstat(fd, &st) < 0) { perror("fstat"); close(fd); return EXIT_FAILURE; }

    size_t filesize = st.st_size;
    size_t mapsize  = (filesize + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    uint64_t *vaddrs = mmap(NULL, mapsize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (vaddrs == MAP_FAILED) { perror("mmap"); close(fd); return EXIT_FAILURE; }
    close(fd);

    // Output the translated physical addresses to an output file
    FILE *out = fopen("output-part2", "wb");
    if (!out) { perror("fopen output-part2"); munmap(vaddrs, mapsize); return EXIT_FAILURE; }

    init_pagetable();
    size_t count = filesize / sizeof(uint64_t);
    for (size_t i = 0; i < count; i++) {
        uint64_t vaddr = vaddrs[i];
        uint64_t vpage = vaddr / PAGE_SIZE;
        uint64_t offset = vaddr % PAGE_SIZE;
        int frame = translate_page(vpage);
        uint64_t paddr = ((uint64_t)frame * PAGE_SIZE) + offset;

        // Detailed output
        printf("Logical Address = 0x%lx :: pageEntry = %lu\t          Frame Number = %d :: Physical address: 0x%lx\n",
               vaddr, vpage, frame, paddr);

        // Write binary physical address
        if (fwrite(&paddr, sizeof(paddr), 1, out) != 1) {
            perror("fwrite"); break;
        }
    }
    // Page-fault summary
    printf("Page fault count = %lu\n", get_page_fault_count());

    fclose(out);
    munmap(vaddrs, mapsize);
    return EXIT_SUCCESS;
}

