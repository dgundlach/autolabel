CC = gcc -Wall -O -g
CFLAGS = -Wall -O -g

BIN = bin
OBJ = obj

DESTDIR =
PREFIX = /usr/local
SYSCONFDIR = /etc
SBINDIR = $(PREFIX)/usr/sbin
#SYSTEMD = /usr/lib/systemd/system

AUTOREMOUNT = $(BIN)/autoremount
AUTOLABEL = $(BIN)/autolabel
TARGETS = $(AUTOREMOUNT) $(AUTOLABEL)
AUTOREMOUNT_OBJS = $(OBJ)/autoremount.o $(OBJ)/daemonize.o $(OBJ)/remount.o $(OBJ)/configure.o
AUTOREMOUNT_LIBS = -lblkid
AUTOLABEL_OBJS = $(OBJ)/autolabel.o $(OBJ)/remount.o $(OBJ)/configure.o
AUTOLABEL_LIBS = -lblkid

all : $(TARGETS)

install: $(TARGETS)
	install -d -m 0755 $(DESTDIR)/$(SYSCONFDIR)
	install -m 0644 altab $(DESTDIR)/$(SYSCONFDIR)
	install -d -m 0755 $(DESTDIR)/$(SBINDIR)
	install -m 0755 $(TARGETS) $(DESTDIR)/$(SBINDIR)

clean:
	rm -f $(OBJ)/*.o $(TARGETS)

$(AUTOREMOUNT): $(AUTOREMOUNT_OBJS)
	$(CC) -o $@ $^ $(AUTOREMOUNT_LIBS)

$(AUTOLABEL): $(AUTOREMOUNT_OBJS)
	$(CC) -o $@ $^ $(AUTOLABEL_LIBS)

$(OBJ)/%.o: %.c
	$(CC) -o $@ -c $^
