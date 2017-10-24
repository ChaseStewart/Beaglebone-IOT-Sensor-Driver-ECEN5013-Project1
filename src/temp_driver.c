#include <stdio.h>
#include "main.h"
#include "temp_driver.h"


void *mainTempDriver(void *arg)
{
	printf("Initializing Temp Driver\n");
	initTempDriver();
	printf("Destroyed Temp Driver\n");

	return NULL;
}

/* Function to configure the temp sensor */
int8_t initTempDriver(void)
{
	printf("Setup Temp Driver\n");
	sendHeartbeatTemp();
	return 0;
}

/* Function to read from the temp register */
int8_t readTempRegisters(uint8_t regAddr, uint8_t *retval)
{
	return 0;
}

/* Function to read N-bytes from the temp register, put result in retval, return status */
int8_t readNTempRegisters(uint8_t regAddr, uint8_t *retval, uint8_t length)
{
	return 0;
}

/* Function to write into the Temp Sensor Registers */
int8_t writeTempRegisters(uint8_t regAddr, uint8_t data)
{
	return 0;
}


