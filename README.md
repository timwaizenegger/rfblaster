# rfblaster
This repo contains arduino code and some instructions for building a programmable 433MHz transmitter for IoT/Smarthome applications.

The cheapest and most widely available smarthome devices are plug receivers, light socket, switches, ... that all use 433MHz (or 866MHz). 
They usually have a small remote control that sends our some simple code that the devices recognizes. It's one-way communication based on matching a preset code pattern.
The rfblaster can send most/arbitrary codes but it's up to you to implement the codes for the devices you use. 

## Where to get the rf codes?
Check the slides for more infos: [doc/04-smarthome.pdf](doc/04-smarthome.pdf)


the source includes some codes for 3 different receiver brands that I own. You can try those first. I found the following works well for figuring out new codes: 

* connect an oscilloscope (digital storage) to the antenna of the remote for your receivers and record the signal. 
* alternative: you can use a 433MHz receiver (usually included with the transmitters you need; see slides) and record it's output with a singal analyzer; even GPIO in of arduino should work. Getting the timing right is more difficult this way though.
* Write down the code... They are very short and you can clearly see how 1/0 are transmitted.
* implement methods for sending 1 and 0 (some use ternary codes; 0, 1 and 2)
* save the code as 0/1/(2) sequences and generate the whole signal with the correct timing

## Versions
* [rfblaster-usb](rfblaster-usb/) uses an arduino and is controlled by RS232 (serial) commands via USB 
* [rfblaster-mqtt](rfblaster-mqtt/) uses an ESP8266 wifi module and connects to an mqtt broker to receive commands


![rfblaster](doc/1.JPG)
![rfblaster](doc/2.JPG)
![rfblaster](doc/3.JPG)
![rfblaster](doc/4.JPG)
![rfblaster](doc/5.JPG)
![rfblaster](doc/6.png)
![rfblaster](doc/7.png)
![rfblaster](doc/8.png)
