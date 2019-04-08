CC=gcc
CFLAGS= -Wall -std=c99
EXEC=bomberman
OBJECT = menu.o monprojet.o terminal.o
all: $(EXEC)
main.o :   menu.c monprojet.c terminal.c
%.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS)
bomberman: $(OBJECT)
	$(CC) -o $@ $(OBJECT) $(CFLAGS)
clean :
		rm $(EXEC) $(OBJECT)
