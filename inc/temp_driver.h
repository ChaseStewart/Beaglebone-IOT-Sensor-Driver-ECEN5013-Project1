#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
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

/*Send heartbeat to main*/
#define sendHeartbeatTemp(x) sendHeartbeat(x, TEMP_DRIVER_ID)

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

/*Function to convert the Tmp102 ADC values to Temperature*/
int16_t tempConversion(int16_t temp);

/* Function to write into the Temp Sensor Registers */
int8_t writeTempRegisters(uint8_t data);

/* Function to write N-bytes into the Temp Sensor Registers */
int8_t writeNTempRegisters(uint8_t* data, size_t length);

/*Function to write the Config Register, returns -1 on error and 0 on success*/
int8_t writeConfig(int16_t config);

/*Function to write the Temp Low Register, returns -1 on error and 0 on success*/
int8_t writeTempLow(int16_t temp);

/*Function to write the Temp High Register, returns -1 on error and 0 on success*/
int8_t writeTempHigh(int16_t temp);

/* Function to read from the temp register */
int8_t readTempRegisters(uint8_t regAddr, uint8_t * readData);

/* Function to read N-bytes from the temp register, put result in retval, return status */
int8_t readNTempRegisters(uint8_t regAddr, uint8_t* readData, size_t length);

/*Function to read the temperature, returns -1 on error and 0 on success*/
int8_t readTemperature(int16_t* temp);

/*Function to read the Config Register, returns -1 on error and 0 on success*/
int8_t readConfig(int16_t* config);

/*Function to read the low temperature threshold, returns -1 on error and 0 on success*/
int8_t readTempLow(int16_t* temp);

/*Function to read the high temperature threshold, returns -1 on error and 0 on success*/
int8_t readTempHigh(int16_t* temp);

/*Function to enable or disable shutdown, False-Disable, True - Enable*/
int8_t enableShutdown(bool shutdownCmd);

/*Function to set the conversion rate*/
int8_t configConvRate(uint16_t convRate);

/*Function to give Temperature in requested values*/
int8_t currentTemperature(int16_t* temp, TEMPUNIT_t units);

int8_t logFromTemp(mqd_t queue, int prio, char *message);

#endif 
