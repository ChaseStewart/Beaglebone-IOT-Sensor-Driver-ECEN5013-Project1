#include <stdio.h>
#include "logger.h"
#include "main.h"

void * mainLogger(void *arg)
{
	printf("Created Logger\n");
	
	printf("Destroyed Logger\n");
	return NULL;
}

