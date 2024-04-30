myshell: myshell.o argparse.o builtin.o
	gcc -o myshell -g myshell.o argparse.o builtin.o

myshell.o: argparse.h builtin.h
	gcc -c -g -Wall myshell.c

builtin.o: builtin.c builtin.h
	gcc -c -g -Wall builtin.c

argeparse.o: argparse.c argparse.h
	gcc -c -g -Wall argparce.c

clean: 
	rm -f *.o *~* *.exe myshell