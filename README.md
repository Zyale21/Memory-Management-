# Virtual Memory Management & Page Table Simulator

This project explores **virtual memory management** concepts within an operating system by simulating paging and address translation in C.

## Features
- **Fixed Page-Table Simulator**  
  - Translates 12-bit virtual addresses (4KB virtual space) to 1KB of physical memory.  
  - Uses 128-byte pages/frames with 7-bit offsets.  
  - Outputs logical-to-physical address mappings.

- **Generalized Page Table Simulator (`assign3part3.c`)**  
  - Accepts user-defined parameters for page size, virtual memory size, physical memory size, and input sequence file.  
  - Works across different machine configurations for greater flexibility.  
  - Demonstrates dynamic handling of paging and memory translation.
