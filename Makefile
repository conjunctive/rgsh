.POSIX:
CC       = cc
CFLAGS   = -ansi -Wpedantic -Wall -Wextra -Wshadow
CPPFLAGS =
LDFLAGS  =
LDLIBS   =
PREFIX   = /usr/local

all: rgsh

rgsh: rgsh.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ rgsh.c $(LDLIBS)

install: rgsh
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 rgsh $(DESTDIR)$(PREFIX)/bin/

test: rgsh
	echo "one\ntwo\nthree\nfour" | ./rgsh "f*r" "ls -al" | ./rgsh "Makefi*" "grep -IR main ." | ./rgsh "a*c" "echo \"PASS\""

clean:
	rm rgsh
