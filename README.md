# Beaglebone IOT Sensor Driver ECEN5013-Project1
This is the Repository for AdvPES project1

## Authors 
Raj Kumar Subramaniam 
Chase E Stewart

## Description
This project provides a client-server architecture to return data from two sensors into a log.
This project is comprised of server program with several independent task pthreads that communicate via mqueues, and a client program that uses the same queues and signal handling to control the server program.
It utilizes Beaglebone's I2C driver to control the Temperature and Light sensors.

## Setup
to build the program, download this repository onto a Beaglebone Black, then use the following commands:
```
make all
make client
```

### Sensors
Connect the two sensors via the Beaglebone Black I2C pins:
* Temperature Sensor: [TMP102](https://www.sparkfun.com/products/13314)
* Light Sensor: [APDS-9301](https://www.sparkfun.com/products/14350)

## Usage
To run the program, run `./project1 -f <logfile_name> &`
and then `.\client` on the same Beaglebone Black. Make sure both sensors are connected to the BBB

It will print the following:
```
beginning
send SIGUSR1 to bring up command interface
send SIGINT to close the program
```

From there, you can send `SIGUSR1` to see the following menu:
```
Welcome to SIGUSR1\n");
Press 'a' to request temp, press 'b' to request light
Press 'c' to log a custom string, press 'd' to close
```
Pressing the corresponding characters will do what you expect. All command output will be logged at the website first provided in `./project1 -f <logfile_name> &`

Similarly, sending SIGINT AKA Ctrl+C will close the client. The server will keep running even if the Client is initialized and closed several times.
