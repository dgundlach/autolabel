CC = gcc -Wall -O -g
CFLAGS = -Wall -O -g

BINDIR = bin
OBJDIR = obj

DESTDIR =
PREFIX = /usr/local
SYSCONFDIR = /etc
SBINDIR = $(PREFIX)/usr/sbin
#SYSTEMD = /usr/lib/systemd/system

AUTOREMOUNT = $(BINDIR)/autoremount
AUTOLABEL = $(BINDIR)/autolabel
TARGETS = $(AUTOREMOUNT) $(AUTOLABEL)
AUTOREMOUNT_OBJS = $(OBJDIR)/autoremount.o $(OBJDIR)/daemonize.o $(OBJDIR)/remount.o $(OBJDIR)/configure.o
AUTOREMOUNT_LIBS = -lblkid
AUTOLABEL_OBJS = $(OBJDIR)/autolabel.o $(OBJDIR)/remount.o $(OBJDIR)/configure.o
AUTOLABEL_LIBS = -lblkid

all : $(TARGETS)

install: $(TARGETS)
	install -d -m 0755 $(DESTDIR)$(SYSCONFDIR)
	install -m 0644 altab $(DESTDIR)$(SYSCONFDIR)
	install -d -m 0755 $(DESTDIR)$(SBINDIR)
	install -m 0755 $(TARGETS) $(DESTDIR)$(SBINDIR)

clean:
	rm -f *.o $(TARGETS)

$(AUTOREMOUNT): $(AUTOREMOUNT_OBJS)
	$(CC) -o $@ $^ $(AUTOREMOUNT_LIBS)

$(AUTOLABEL): $(AUTOREMOUNT_OBJS)
	$(CC) -o $@ $^ $(AUTOLABEL_LIBS)

$(OBJDIR)/%.o: %.c
	$(CC) -o $@ -c $^
