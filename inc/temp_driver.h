#include <stdint.h>

#define TEMP_SLAVE_ADDR 0x00 // TODO FIXME


/* Main function for the temperature driver*/
void *mainTempDriver(void *);

/* Function to configure the temp sensor */
int8_t initTempDriver(void);

/* Function to read from the temp register */
int8_t readTempRegisters(uint8_t regAddr, uint8_t *retval);

/* Function to read N-bytes from the temp register, put result in retval, return status */
int8_t readNTempRegisters(uint8_t regAddr, uint8_t *retval, uint8_t length);

/* Function to write into the Temp Sensor Registers */
int8_t writeTempRegisters(uint8_t regAddr, uint8_t data);

/* Send heartbeat to main */
int8_t sendHeartbeat(void);

