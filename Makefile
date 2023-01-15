COMP_ARGS = -ansi -Wall -pedantic-errors

ifdef DEBUG
	COMP_ARGS += -g
endif

all:
	gcc $(COMP_ARGS) -o mes main.c parser.c utils.c tokenizer.c

lib:
	gcc $(COMP_ARGS) -c -fpic parser.c
	gcc $(COMP_ARGS) -c -fpic tokenizer.c
	gcc $(COMP_ARGS) -c -fpic utils.c
	gcc -shared -o libmes.so parser.o tokenizer.o utils.o
