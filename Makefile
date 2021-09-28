.POSIX:
.SUFFIXES:

DESTDIR	= /usr/local/bin
RM	= rm -rf

all: shutdownd
clean: ; $(RM) shutdownd
debug: ; $(MAKE) clean shutdownd CFLAGS="$(CFLAGS) -DEBUG"

uninstall: ; $(RM) $(DESTDIR)/shutdownd
install: shutdownd
	cp shutdownd $(DESTDIR)/shutdownd
	sudo chmod 4755 $(DESTDIR)/shutdownd

.SILENT: debug
.SUFFIXES: .c
