all:
	$(MAKE) -C sys_v
	$(MAKE) -C posix

clean:
	$(MAKE) clean -C sys_v
	$(MAKE) clean -C posix
