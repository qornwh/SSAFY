
main : ssd.o
	gcc ssd.o -o ssd

ssd.o : common.h ssd.c
	gcc -c common.h ssd.c

clean :
	rm ssd.o ssd *.h.gch core.*