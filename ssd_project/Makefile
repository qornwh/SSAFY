
main : ssd.o
	gcc ssd.o -o ssd.out

ssd.o : common.h ssd.c
	gcc -c common.h ssd.c

testShell : common.h testShell.c
	gcc common.h testShell.c -o testShell.out

clean :
	rm ssd.o ssd *.h.gch core.*