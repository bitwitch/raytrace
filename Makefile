src = $(wildcard *.cpp)
obj = $(src:.cpp=.o)

CC = gcc 
CFLAGS = -Wall -O2 

ray: $(obj)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) ray
