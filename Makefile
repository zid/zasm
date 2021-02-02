all: zasm

zasm: main.o assemble.o
	gcc main.o assemble.o -flto -O3 -o zasm

%.o : %.c
	gcc $^ -c -W -Wall -O3
