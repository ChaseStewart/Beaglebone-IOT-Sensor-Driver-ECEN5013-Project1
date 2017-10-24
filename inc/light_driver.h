#include <stdint.h>

#define LIGHT_SLAVE_ADDR 0x00 // TODO FIXME


/* Main function for the light driver */
void *mainLightDriver(void *);

/* Function to configure the light sensor */
int8_t initLightDriver(void);

/* Function to read from the light register */
int8_t readLightRegisters(uint8_t regAddr, uint8_t *retval);

/* Function to read N-bytes from the light register, put result in retval, return status */
int8_t readNLightRegisters(uint8_t regAddr, uint8_t *retval, uint8_t length);

/* Function to write into the Light Sensor Registers */
int8_t writeLightRegisters(uint8_t regAddr, uint8_t data);

/* Send heartbeat to main */
int8_t sendHeartbeat(void);

