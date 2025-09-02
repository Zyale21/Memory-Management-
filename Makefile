#Zyale Brown-Sanger
#added the mv so the output file goes to the result directory

all: analyzesequence assign3part1 assign3part2 assign3part3

analyzesequence:
	gcc -o analyzesequence analyzesequence.c

assign3part1: assign3part1.c
	gcc -g -o assign3part1 assign3part1.c

assign3part2: assign3part2.c phyframes.c pagetable.c
	gcc -g -o assign3part2 assign3part2.c phyframes.c pagetable.c

assign3part3: assign3part3.c phyframes.c pagetable.c
	gcc -g -o assign3part3 assign3part3.c phyframes.c pagetable.c

clean:
	rm -f assign3part1 assign3part2 assign3part3
	rm -f analyzesequence
	rm -f result/*
	rm -f output-*
test:
	./assign3part1 data/part1testsequence

part1:
	./assign3part1 data/part1sequence
	mv output-part1 result/
part2:
	./assign3part2 data/part2sequence
	mv output-part2 result/
part3:
	./assign3part3 128 4096 1024 data/part2sequence
	./assign3part3 256 4096 1024 data/part2sequence
	./assign3part3 128 4096 2048 data/part2sequence
	mv output-part3 result/
