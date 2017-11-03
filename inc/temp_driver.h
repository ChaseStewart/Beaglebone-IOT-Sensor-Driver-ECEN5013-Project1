#include <stdint.h>
#include <mqueue.h>

#ifndef __MY_TEMP_DRIVER_H__
#define __MY_TEMP_DRIVER_H__

#define I2C_FILE 				"/dev/i2c-2"	/*I2C device location*/
#define TEMP_SLAVE_ADDRESS 		0x48			/*Slave Address*/
#define TEMP_SENSOR_RESOLUTION 	0.0625			/*ADC Conversion Resolution*/
#define READ_BIT 				1
#define WRITE_BIT 				0

#define PTR_ADDRESS_TEMP 	0x00 	/*Temperature REgister*/
#define PTR_ADDRESS_CONFIG 	0x01	/*Congig Register*/
#define PTR_ADDRESS_TLOW 	0x02	/*Low Threshold Register*/
#define PTR_ADDRESS_THIGH 	0x03 	/*High Threshold Register*/

#define BIT_SHUTDOWN_16BIT	0x0100	/*Shutdown Bits set <Byte1Byte2> format*/
#define BIT_CONV_RATE_0_25HZ	0x00 	/*Conversion Rate*/
#define BIT_CONV_RATE_1HZ		0x40
#define BIT_CONV_RATE_4HZ		0x80
#define BIT_CONV_RATE_8HZ		0xC0

/*Enum for Temp Unit Conversions*/
typedef enum
{
	UNIT_CELCIUS = 0,
	UNIT_FAHRENHEIT,
	UNIT_KELVIN
}TEMPUNIT_t;

/* Main function for the temperature driver*/
void *mainTempDriver(void *);

/* Function to configure the queues */
int8_t initTempQueues(mqd_t *main_queue, mqd_t *logger_queue, mqd_t *temp_queue);

/* Function to configure the temp sensor */
int8_t initTempDriver(void);

/* Function to read from the temp register */
int8_t readTempRegisters(uint8_t regAddr, uint8_t *retval);

/* Function to read N-bytes from the temp register, put result in retval, return status */
int8_t readNTempRegisters(uint8_t regAddr, uint8_t *retval, uint8_t length);

/* Function to write into the Temp Sensor Registers */
int8_t writeTempRegisters(uint8_t regAddr, uint8_t data);

/*Send heartbeat to main*/
#define sendHeartbeatTemp(x) sendHeartbeat(x, TEMP_DRIVER_ID)

#endif 
