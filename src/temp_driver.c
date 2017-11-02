#include <stdio.h>
#include "temp_driver.h"
#include "common.h"
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>

int32_t i2cHandle;	/*File Descriptor for I2C access*/


void *mainTempDriver(void *arg)
{
	mqd_t main_queue, logger_queue, temp_queue;
	
	/* Create queue for main thread */
	printf("Initializing Temp Queues\n");
	initTempQueues(&main_queue, &logger_queue, &temp_queue);
	
	printf("Initializing Temp Driver\n");
	initTempQueues(&main_queue, &logger_queue, &temp_queue);

	while(temp_state ==STATE_RUNNING)
	{
		pthread_cond_wait(&temp_cv, &temp_mutex);
		printf("Temp Driver Awake!\n");
	}

	printf("Destroyed Temp Driver\n");

	return NULL;
}

/* Function to configure the temp sensor */
int8_t initTempQueues(mqd_t *main_queue, mqd_t *logger_queue, mqd_t *temp_queue)
{
	/* Create main queue*/
	printf("Creating queue \"%s\"\n", MAIN_QUEUE_NAME);
	(*main_queue) = mq_open(MAIN_QUEUE_NAME, O_CREAT | O_WRONLY, 0755, NULL);
	if ((*main_queue) == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}
	
	/* Create main queue*/
	printf("Creating queue \"%s\"\n", LOGGER_QUEUE_NAME);
	(*logger_queue) = mq_open(LOGGER_QUEUE_NAME, O_CREAT | O_WRONLY, 0755, NULL);
	if ((*logger_queue) == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}
	/* Create main queue*/
	printf("Creating queue \"%s\"\n", TEMP_DRIVER_QUEUE_NAME);
	(*temp_queue) = mq_open(TEMP_DRIVER_QUEUE_NAME, O_CREAT | O_RDONLY, 0755, NULL);
	if ((*temp_queue) == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}
	return 0;
}

/* Function to configure the temp sensor */
int8_t initTempDriver(void)
{
	printf("Setup Temp Driver\n");
	//sendHeartbeatTemp();
	
	if ((i2cHandle = open(I2C_FILE,O_RDWR)) < 0)
	{
		printf("Error Opening I2C device - Temp Sensor\n");
		exit(1);
	}

	if(ioctl(i2cHandle,I2C_SLAVE, SLAVE_ADDRESS) < 0)
	{
		printf("Failed to talk to Slave and Acquire bus access\n");
		exit(1);
	}
	return 0;
}

/*Function to convert the Tmp102 ADC values to Temperature*/
int16_t tempConversion(int16_t temp)
{
	temp = temp >>4;
	if(temp & 0x400) /*finding Negative values*/
	{
		temp = ~temp;	/*Finding Absolute Value by 2s Complement and multpily by -1*/
		temp++;
		return (int16_t)((float)temp*TEMP_SENSOR_RESOLUTION*(-1.0));
	}
	else	/*If Positive Values*/
	{
		return (int16_t)((float)temp*TEMP_SENSOR_RESOLUTION);
	}
	return -1;
}

/* Function to write into the Temp Sensor Registers */
int8_t writeTempRegisters(uint8_t data)
{
	if(write(i2cHandle, &data, 1) != 1)
	{
		printf("I2C Write error seen\n");
		return -1;
	}
	return 0;
}

/* Function to write N-bytes into the Temp Sensor Registers */
int8_t writeNTempRegisters(uint8_t* data, size_t length)
{
	if(write(i2cHandle, data, length) != length)	/*Writes N-bytes into file*/
	{
		printf("I2C Write error seen\n");
		return -1;
	}
	return 0;
}

/*Function to write the Config Register, returns -1 on error and 0 on success*/
int8_t writeConfig(int16_t config)
{
	int8_t status;
	int8_t buffer[3];
	int8_t* value = (int8_t*)&config;
	buffer[0] = PTR_ADDRESS_CONFIG;	/*Register Address*/
	buffer[1] = value[1];			/*Byte1*/
	buffer[2] = value[0];			/*Byte2*/
	status = writeNTempRegisters(buffer,3);	
	return status;
}

/*Function to write the Temp Low Register, returns -1 on error and 0 on success*/
int8_t writeTempLow(int16_t temp)
{
	int8_t status;
	int8_t buffer[3];
	int8_t* value = (int8_t*)&temp;
	buffer[0] = PTR_ADDRESS_TLOW;	/*Register Address*/
	buffer[1] = value[1];			/*Byte1*/
	buffer[2] = value[0];			/*Byte2*/
	status = writeNTempRegisters(buffer,3);
	return status;
}


/*Function to write the Temp High Register, returns -1 on error and 0 on success*/
int8_t writeTempHigh(int16_t temp)
{
	int8_t status;
	int8_t buffer[3];
	int8_t* value = (int8_t*)&temp;
	buffer[0] = PTR_ADDRESS_THIGH;	/*Register Address*/
	buffer[1] = value[1];			/*Byte1*/
	buffer[2] = value[0];			/*Byte2*/
	status = writeNTempRegisters(buffer,3);
	return status;
}

/* Function to read from the temp register */
int8_t readTempRegisters(uint8_t regAddr, uint8_t * readData)
{
	writeTempRegisters(regAddr);		/*Writing into Pointer Register helps in reading the corresponding register*/	
	if(read(i2cHandle, readData, 1) != 1)
	{
		printf("Less Number of bytes are received/read error\n");
		return -1;
	}
	return 0;
}

/* Function to read N-bytes from the temp register, put result in retval, return status */
int8_t readNTempRegisters(uint8_t regAddr, uint8_t* readData, size_t length)
{
	char data[length];
	writeTempRegisters(regAddr);	/*Writing into Pointer Register helps in reading the corresponding register*/
	if(readData == NULL)
	{
		return -1;
	}
	if(read(i2cHandle, data, length) != length)	/*Stores read value*/
	{
		printf("Less Number of bytes are received/read error\n");
		return -1;
	}
	/*If the 16 bit value is readData[0] = 0x34, readData[1] = 0x12 so, typecasting into int16_t gives 0x1234 
	 *This is little Endian issue and so, the bytes has to be reversed while storing in the 16-bit varible*/ 
	for(int32_t i = length-1, j=0; i >= 0;i--, j++)
	{
		readData[j] = data[i];
	}
	return 0;
}

/*Function to read the temperature, returns -1 on error and 0 on success*/
int8_t readTemperature(int16_t* temp)
{
	int8_t status;
	status = readNTempRegisters(PTR_ADDRESS_TEMP,(uint8_t*)temp,2);
	*temp = tempConversion(temp);
	return status;
}

/*Function to read the Config Register, returns -1 on error and 0 on success*/
int8_t readConfig(int16_t* config)
{
	int8_t status;
	status = readNTempRegisters(PTR_ADDRESS_CONFIG,(uint8_t*)config,2);
	return status;
}

/*Function to read the low temperature threshold, returns -1 on error and 0 on success*/
int8_t readTempLow(int16_t* temp)
{
	int8_t status;
	status = readNTempRegisters(PTR_ADDRESS_TLOW,(uint8_t*)temp,2);
	return status;
}

/*Function to read the high temperature threshold, returns -1 on error and 0 on success*/
int8_t readTempHigh(int16_t* temp)
{
	int8_t status;
	status = readNTempRegisters(PTR_ADDRESS_THIGH,(uint8_t*)temp,2);
	return status;
}


