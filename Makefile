CC = gcc
CFLAGS = -std=c99 -Wall -g 
LFLAGS = -ledit

PROGS = lisp_v001

all: $(PROGS)
	rm -f *.o

$(PROGS): % : %.o
	$(CC) $(LFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -f *.o $(PROGS)