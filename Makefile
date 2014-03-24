all:
	gcc -O3 solve.c -lm -o game
assembly:
	gcc -O3 solve.c -lm -S

