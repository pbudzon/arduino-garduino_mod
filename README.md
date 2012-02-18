# Garduino mod

This is a modification of the original "Garduino" project. 

It reads the data from the sensors attached to the plants and sends them back to our server, where they are saved and processed.
It is using an Arduino Wiznet Ethernet shield.

This work is based on the "Garduino" project made originally by Luke Iseman and published in MAKE magazine: http://makeprojects.com/Project/Garduino-Geek-Gardening/62/1 

Moisture sensors used are exactly the same as in the above project.

## Circuit:
* Ethernet shield attached to pins 10, 11, 12, 13
* Analog inputs:
 + moisture sensors on pins 4 and 5,
 + light sensor on pin 1,
 + temperature sensor on pin 0.