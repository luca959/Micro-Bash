OBJS	= ubashFC.o funzioni.o
SOURCE	= ubashFC.c funzioni.c
HEADER	= header.h
OUT	= ubash

all: ubash

ubash: $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c $(HEADER)
	$(CC) -c -o $@ $< $(LFLAGS)

clean:
	rm -f $(OBJS) $(OUT)

run: $(OUT)
	./$(OUT)
