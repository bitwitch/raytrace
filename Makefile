src = $(wildcard *.cpp)
obj = $(src:.cpp=.o)

CC = gcc 
CFLAGS = -Wall -g -ggdb

ray: $(obj)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) ray
