CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -Wpedantic
PKGS = gtk4 libadwaita-1
CFLAGS += $(shell pkg-config --cflags $(PKGS))
LDLIBS += $(shell pkg-config --libs $(PKGS))

.PHONY: all clean check
all: faliz-shell faliz-settings faliz-installer

faliz-shell: main.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

faliz-settings: settings.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

faliz-installer: installer.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

check: faliz-shell faliz-settings faliz-installer
	./faliz-shell --version
	./faliz-settings --version
	./faliz-installer --version

clean:
	rm -f faliz-shell faliz-settings faliz-installer
