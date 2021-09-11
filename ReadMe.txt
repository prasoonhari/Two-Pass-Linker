A C++ Simulation of a two-pass linker.

A linker takes individually compiled code/object modules and creates a single executable by resolving external symbol references (e.g. variables and functions) and module relative addressing by assigning global addresses after placing the modules’ object code at global addresses.
Rather than dealing with complex x86 tool chains, I have assumed a target machine with the following properties: (a) word addressable, (b) addressable memory of 512 words, and (c) each valid word is represented by an integer (<10000).

The input to the linker is a file containing a sequence of tokens.


Files:

ReadMe.txt 
Tokenizer.cpp - tokenizer class defined to resolving external symbol references and tokenize input.
Tokenizer.h - tokenizer header file. 
linker.cpp - main class (use tokenizer class)
MakeFile - creates linker executable file.


General Note:

Please give all the files the permission it required. (if any)
Not specified any g++ standard.
Please run "make clean", if compiling again using makefile.

