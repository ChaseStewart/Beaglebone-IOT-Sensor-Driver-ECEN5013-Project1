#include <stdio.h>
#include "light_driver.h"
#include "main.h"

void * mainLightDriver(void *arg)
{
	printf("Initializing LightDriver\n");
	initLightDriver();
	printf("Destroyed LightDriver\n");
	return NULL;
}

/* Function to configure the light sensor */
int8_t initLightDriver(void)
{
	printf("Setup LightDriver\n");
	sendHeartbeatLight();
	return 0;
}

/* Function to read from the light register */
int8_t readLightRegisters(uint8_t regAddr, uint8_t *retval)
{
	return 0;
}

/* Function to read N-bytes from the light register, put result in retval, return status */
int8_t readNLightRegisters(uint8_t regAddr, uint8_t *retval, uint8_t length)
{
	return 0;
}

/* Function to write into the Light Sensor Registers */
int8_t writeLightRegisters(uint8_t regAddr, uint8_t data)
{
	return 0;
}


