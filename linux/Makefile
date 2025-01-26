# choose your compiler, e.g. gcc/clang
# example override to clang: make run CC=clang
CC = gcc

# the most basic way of building that is most likely to work on most systems
.PHONY: hello-llm
run: main.c llm.c
	$(CC) -O2 -std=gnu11  -o hello-llm main.c llm.c -lm

.PHONY: clean
clean:
	rm -f hello-llm
