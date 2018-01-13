default: all
all:a1shell
a1shell:a1shell.c
	gcc -o a1shell a1shell.c
clean:
	rm -f a1shell
	rm -f alshell.o
tar:
	tar zcvf submit.tar *