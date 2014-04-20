PROJ=device_probe

CC=gcc

CFLAGS=-std=c99 -Wall -g -pedantic

# Check for 32-bit vs 64-bit
PROC_TYPE = $(strip $(shell uname -m | grep 64))
 
LIBS=-lOpenCL

$(PROJ): $(PROJ).c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

.PHONY: clean

clean:
	rm $(PROJ)
