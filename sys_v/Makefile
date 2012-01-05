all: utils.o
	$(MAKE) -C I
	$(MAKE) -C II
	$(MAKE) -C III

utils.o: utils.c
	gcc -Wall -c utils.c -o utils.o

clean:
	$(MAKE) clean -C I
	$(MAKE) clean -C II
	$(MAKE) clean -C III
