CC = gcc

CFLAGS = -g -std=c99 -Werror -Wall

INCS = -I./inc

LIBS = -pthread

SRCFILES =  src/main.c
SRCFILES += src/light_driver.c
SRCFILES += src/temp_driver.c
SRCFILES += src/logger.c


all: 
	$(CC) $(CFLAGS) $(INCS) $(SRCFILES) $(LIBS) -o project1
	@echo "Project 1 build complete!"

client:
	$(CC) $(CFLAGS) $(INCS) src/client.c $(LIBS) -o client
	@echo "Project 1 build complete!"


.PHONY: clean

clean:
	@rm -vf project1
	@echo project removed
	@rm -vf client
	@echo client removed

