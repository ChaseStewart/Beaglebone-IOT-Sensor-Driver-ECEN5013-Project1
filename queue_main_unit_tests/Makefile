CC=gcc

CFLAGS= -g -std=c99

IPATH  = -I../inc 
IPATH  = -I./ 
IPATH += -I../Unity/src

LIBS= -pthread -lrt

FILES  = unit_test.c
FILES += ../Unity/src/unity.c

all:
	$(CC) $(CFLAGS) $(IPATH) $(FILES) $(LIBS) -o unit_test
	@echo Test Build Complete!

.PHONY: clean

clean:
	@rm -vf test
	@echo Test Executable Removed!
