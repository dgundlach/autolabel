CC = gcc -Wall -O -g
TARGETS = autoremount autolabel
DESTDIR =
prefix = /usr/local
sbindir = $(PREFIX)/usr/sbin
#systemd = /usr/lib/systemd/system
sysconfdir = /etc

all : $(TARGETS)

install: $(TARGETS)
	install -d -m 0755 $(DESTDIR)$(sysconfdir)
	install -m 0644 altab $(DESTDIR)$(sysconfdir)
	install -d -m 0755 $(DESTDIR)$(sbindir)
	install -m 0755 $(TARGETS) $(DESTDIR)$(sbindir)
	install -m 0755 autolabel $(DESTDIR)$(sbindir)

clean:
	rm -f *.o $(TARGETS)

autoremount: autoremount.o daemonize.o remount.o configure.o
	$(CC) -o $@ $^ -lblkid

autolabel: autolabel.o remount.o configure.o
	$(CC) -o $@ $^ -lblkid

%.o: %.c
	$(CC) -o $@ -c $^
