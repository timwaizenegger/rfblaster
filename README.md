# rfblaster
This repo contains arduino code and some instructions for building a programmable 433MHz transmitter for IoT/Smarthome applications.

## Versions
* [rfblaster-usb](rfblaster-usb/) uses an arduino and is controlled by RS232 (serial) commands via USB 
* [rfblaster-mqtt](rfblaster-mqtt/) uses an ESP8266 wifi module and connects to an mqtt broker to receive commands

The USB/MQTT commands tell the rfblaster which 422 rf commands should be sent. These commands can control plug sockets etc.

![rfblaster](doc/1.JPG)
![rfblaster](doc/2.JPG)
![rfblaster](doc/3.JPG)
![rfblaster](doc/4.JPG)
![rfblaster](doc/5.JPG)
