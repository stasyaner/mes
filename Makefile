all:
	gcc -ansi -Wall -pedantic-errors -g -o mes main.c parser.c utils.c tokenizer.c
