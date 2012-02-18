/*
 Garduino mod

 This is a modification of the original "Garduino" project. 
 It reads the data from the sensors attached to the plants and sends
 them back to our server, where they are saved and processed,
 using an Arduino Wiznet Ethernet shield.
 
 This work is based on the "Garduino" project made
 originally by Luke Iseman and published in MAKE magazine:
 http://makeprojects.com/Project/Garduino-Geek-Gardening/62/1
 Moisture sensors used are exactly the same as in the above project.
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs:
   ** moisture sensors on pins 4 and 5,
   ** light sensor on pin 1,
   ** temperature sensor on pin 0/
 
 created: February 2012
 by Paulina Budzoń.

*/

#include <SPI.h>
#include <Ethernet.h>

//define analog inputs to which we have connected our sensors
const int moistureSensor_1 = 5;   //1st moisture sensor on pin 5
const int moistureSensor_2 = 4;   //2nd moisture sensor on pin 4
const int lightSensor = 1;        //light sensor on pin 1
const int tempSensor = 0;         //temperature sensor on pin 0

//define variables to store moisture, light, and temperature values
int moisture_val_1;     //final value of the 1st moisture sensor
int moisture_val_2;     //final value of the 2nd moisture sensor
int light_val;          //value from the light sensor
int temp_val;           //value from the temperature sensor
float temp;             //actual temperature calculated from the temperature sensor
int moistures_1 = 0;    //holder for the averaging value from the 1st moisture sensor
int moistures_2 = 0;    //holder for the averaging value from the 2nd moisture sensor

//variables used for handling ethernet connection
String currentResponse = "";    //string to saving current reponse from the server
int data_sent = 0;              //number of tries that were used to send the data
const int try_sent_max = 5;           //maximum number of tries we will use to try to send the data

//stuff for connections
byte mac[] = {  0x00, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  //mac address for the ethernet shield
IPAddress ip(10,0,1,39);                               //fallback ip address to use, when dhcp fails
char serverName[] = "inka.rstack.cc";                  //address of the server we will send data to
EthernetClient client;       

void setup() {
  Serial.begin(9600);
  
  Serial.println("Starting ethernet shield...");
  // attempt a DHCP connection:
  if (Ethernet.begin(mac) == 0) { //if dhcp fails, fallback to static ip
      Serial.println("Failed to get IP from DHCP, setting static");
      Ethernet.begin(mac, ip);
  }
  Serial.print("All fine, got IP: ");
  Serial.println(Ethernet.localIP());
};

void loop () { 
  if (client.connected()) { //if connection is made:
    if (client.available()) { //if there is more response to read, read it
      char c = client.read();
      currentResponse += c; 
    }
    else { //if there is no more response to read:
      //we can do something with response here (nothing now)
      //maybe check if the response is what we expect, to make sure data was saved properly.    
      Serial.println(currentResponse); //print the response to serial
      Serial.println();
      Serial.println("disconnecting.");
      client.stop(); //close the connection
      currentResponse = "";
      delay(300000); //wait 5 minutes for next run
    }
  }
  else{ //if there is no connection - read sensors and send the data
    Serial.print("reading moisture sensors...");    
    for(int i = 1; i <= 5; i++){ //take 5 reads from moisure sensors
      Serial.print(" ");
      Serial.print(i);
      Serial.print(" ");
      moistures_1 += analogRead(moistureSensor_1);
      moistures_2 += analogRead(moistureSensor_2);
    }
    Serial.println("reading done");
    
    //calculate average values from moisture sensors
    moisture_val_1 = moistures_1/5;
    moisture_val_2 = moistures_2/5;
    //read the remaining sensors - light and temp
    light_val = analogRead(lightSensor);
    temp_val = analogRead(tempSensor);
    //calculate the actual temperature from the temp sensor
    temp = temp_val*5/1024.0;
    temp = temp - 0.5;
    temp = temp / 0.01;
    
    //reset the average holders for moisture sensors
    moistures_1 = 0;
    moistures_2 = 0;
    
    //we can print the values to serial if we want
//    Serial.print("moist 1 final ");
//    Serial.println(moisture_val_1);
//    Serial.print("moist 2 final ");
//    Serial.println(moisture_val_2);
//    Serial.print("light final ");
//    Serial.println(light_val);
//    Serial.print("temp final ");
//    Serial.println(temp_val);
//    Serial.print("temp converted ");
//    Serial.println(temp);
  
    //send the data to de server
    Serial.println("connecting to server...");
    data_sent++; //save next connection attempt
    if(client.connect(serverName, 80)) { 
        //if connection is made successfully, send the data
        Serial.println("making HTTP request...");
        client.print("GET /receive.php?moist1=");
        client.print(moisture_val_1);
        client.print("&moist2=");
        client.print(moisture_val_2);
        client.print("&light=");
        client.print(light_val);
        client.print("&temp=");
        client.print(temp);
        client.println(" HTTP/1.1");
      
        client.println("HOST: inka.rstack.cc");
        client.println("Authorization: Basic aW5rYTpkdXBhLjEyMw=="); //basic auth for our server
        client.println(); //end headers
       
        data_sent = 0; //reset the counter - connection made sucessfully
        delay(1000); //wait 1 sec before reading the response
      }
      //if the connection was not made successfully and we reached the maximum number of attemps
      if(data_sent >= try_sent_max){
         Serial.println("Reached maximum number of unsucessfull attempts, skipping");
         client.stop();  //close the connection, if any
         data_sent = 0;
         currentResponse = "";
         delay(300000); //wait 5 minutes for next run
      }
  }
 
};
