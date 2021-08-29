.POSIX:

DESTDIR	= /usr/local/bin
RM 	= rm -rf

all: shutdownd
shutdownd: shutdownd.c
clean: ; $(RM) shutdownd

uninstall: ; $(RM) $(DESTDIR)/shutdownd
install: shutdownd
	cp shutdownd $(DESTDIR)/shutdownd
	sudo chmod 4755 $(DESTDIR)/shutdownd

debug:
	$(MAKE) clean shutdownd CFLAGS="$(CFLAGS) -DEBUG"
