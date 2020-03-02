TARBALL= lab3a-605142505.tar.gz
CFLAG= -Wall -Wextra

default:	
	gcc $(CFLAG) -o lab3a lab3a.c

clean:
	rm -f *.tar.gz lab3a

dist:   
	tar -cvzf $(TARBALL) README Makefile lab3a.c
