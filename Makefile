all:
	gcc -O3 solve.c -lm -o game
source:
	gcc -O3 solve.c -lm -S

