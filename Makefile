CFLAGS=-Wall -std=c11
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

ycc: $(OBJS)
	$(CC) -o ycc $(OBJS) $(LDFLAGS)

$(OBJS): ycc.h

clean:
	rm -f ycc *.o 
