CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -Wpedantic
PKGS = gtk4 libadwaita-1
CFLAGS += $(shell pkg-config --cflags $(PKGS))
LDLIBS += $(shell pkg-config --libs $(PKGS))

.PHONY: all clean check
all: faliz-shell

faliz-shell: main.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

check: faliz-shell
	./faliz-shell --version

clean:
	rm -f faliz-shell
