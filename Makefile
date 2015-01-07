all:
	gcc -O3 solve.c -lm -o game -std=c99 -march=native -ffast-math
assembly:
	gcc -O3 solve.c -lm -S

