main: main.o fifo.o spinlock.o tas64.o
	gcc main.o fifo.o spinlock.o tas64.o -o p3

main.o: main.c fifo.h spinlock.h
	gcc -c main.c

fifo.o: fifo.c fifo.h spinlock.h
	gcc -c fifo.c

spinlock.o: spinlock.c spinlock.h
	gcc -c spinlock.c

tas64.o: tas64.S
	gcc -c tas64.S

test.o: test.c spinlock.h
	gcc -c test.c

lock: spinlock.o tas64.o test.o
	gcc test.o spinlock.o tas64.o -o test

clean:
	rm -f main main.o fifo.o spinlock.o tas64.o p3 test test.o