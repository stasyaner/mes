all:
	gcc -ansi -Wall -pedantic-errors -g main.c parser.c utils.c tokenizer.c -o mes
