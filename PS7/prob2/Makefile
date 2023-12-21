as:
	as --64 -o p2.o p2.s
	ld -m elf_x86_64 -o p2_s p2.o
	rm p2.o

cc:
	gcc -o p2_c p2.c

clean:
	rm -f p2_s p2_c
