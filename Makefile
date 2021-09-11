SHELL:=/bin/bash

CC=g++

linker: Tokenizer.cpp linker.cpp
	$(CC) -o linker Tokenizer.cpp linker.cpp -I.

clean:
	rm -f linker *~
