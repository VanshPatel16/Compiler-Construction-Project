# make file for compilation
run : stage1exe
	./stage1exe
all : lexer.c parser.c driver.c
	gcc lexer.c parser.c driver.c -o stage1exe

clean : 
	rm stage1exe