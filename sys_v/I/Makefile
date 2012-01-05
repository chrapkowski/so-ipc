all:
	$(MAKE) -C A
	$(MAKE) -C B
	$(MAKE) -C C

utils.o: utils.c
	gcc -C -Wall utils.c -o utils.o

clean:
	$(MAKE) clean -C A
	$(MAKE) clean -C B
	$(MAKE) clean -C C
