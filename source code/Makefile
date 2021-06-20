# -*- Makefile -*-

#variables
CC=g++

terminal.out: main.o Tokenizer.o CommandExecuter.o 
	$(CC) main.o Tokenizer.o CommandExecuter.o -o terminal.out

Tokenizer.o: Tokenizer.h Tokenizer.cpp 
	$(CC) Tokenizer.cpp -c

CommandExecuter.o: CommandExecuter.h CommandExecuter.cpp
	$(CC) CommandExecuter.cpp -c
	
main.o: main.cpp
	$(CC) main.cpp -c
	
clean:
	rm *.o terminal.out
