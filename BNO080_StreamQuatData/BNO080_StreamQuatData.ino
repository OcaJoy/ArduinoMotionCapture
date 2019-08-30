/*
  Using the BNO080 IMU
  By: Nathan Seidle
  SparkFun Electronics
  Date: December 21st, 2017
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14586

  This example shows how to output the i/j/k/real parts of the rotation vector.
  https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation

  It takes about 1ms at 400kHz I2C to read a record from the sensor, but we are polling the sensor continually
  between updates from the sensor. Use the interrupt pin on the BNO080 breakout to avoid polling.

  Hardware Connections:
  Attach the Qwiic Shield to your Arduino/Photon/ESP32 or other
  Plug the sensor onto the shield
  Serial.print it out at 9600 baud to serial monitor.
*/
#include <Wire.h>
#include "SparkFun_BNO080_Arduino_Library.h"

// Intitialize Global Variables
float quat[4] = {1.0f, 0.0f, 0.0f, 0.0f}; //Float array that holds the quaternion values
int data_length = sizeof(quat); //Length of the quaternion array in Bytes (should be 16 bytes)
int deviceCheck = 0; //This value will be the number of devices that are not responding
byte IMUnum; //This value will contain which IMU is being used for debugging purposes
boolean dataReady = false;
BNO080 myIMU;

void setup()
{
  // Set up Pins for Multiplexer Channel Select
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  // Initiallize Variables
  boolean IMUcheck = true;

  // Begin Serial Communication @ 115200 Baud Rate
  Serial.begin(115200);
  // Begin I2C Communication
  Wire.begin();
  Wire.setClock(400000); //Increase I2C data rate to 400kHz

  // Loop through all the IMUs to check if they are working and to initiallize them
  for(int x=1; x<4; x++)
  {
    // Set the channel for the Multiplexer
    MuxSelector(x);

    // Check to see if current IMU is not responding
    if (myIMU.begin() == false) // if not responding 
    {
      // Set the IMUcheck to false
      IMUcheck = false;
      
      // Set the IMUnum to the number of the current loop it is in
      IMUnum = x;

      // Send byte for python to know it is not responding
      Serial.write(IMUnum);
    }
    else // if IMU is responding
    {
      // Set the IMU to send data update every 50ms
      myIMU.enableRotationVector(50); 
    }  
  }

  // Check if all the IMU are functioning
  if (IMUcheck)
  { 
    // Set the IMUnum to 100 (Code for: everything is fine)
    IMUnum = 100;

    // Send byte for python to konw that Arduino is done setting up
    Serial.write(IMUnum);
  }
  else
  { 
    // Set the IMUnum to 0 (Code for: one or more IMUs are not responding)
    IMUnum = 0;

    // Send byte for python to konw that Arduino that not all IMUs are responding
    Serial.write(IMUnum);
    
    // Freeze arduino when not all IMUs are functioning
    while(1);
  }
}

void loop()
{ 
  // Loop through all the BNO080s to read their data and stream to python
  for(int x=1; x<4; x++)
  { 
    // Select the channel for the Multiplexer
    MuxSelector(x);

    // Keep checking if data is available
    while(myIMU.dataAvailable() == false){}

    quat[1] = myIMU.getQuatI();
    quat[2] = myIMU.getQuatJ();
    quat[3] = myIMU.getQuatK();
    quat[0] = myIMU.getQuatReal();

    // Send the quaternion data
    // Wrap the quaternion structure with a Start("S") Byte and an End("E") Byte to make sure a full structure is received 
    Serial.write('S');
    IMUnum = x; // Set the IMU number to the current BNO080 being used by the loop
    Serial.write(IMUnum); //  Send the IMU num to python
    Serial.write((uint8_t *)&quat, data_length);
    Serial.write('E');
  }
}

// This function set the Multiplexer to connect to the desired BNO080 depending on the input
void MuxSelector(int x)
{
  switch(x)
  {
    case 1:
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      digitalWrite(5, LOW);
      break;

    case 2:
      digitalWrite(2, HIGH);
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      digitalWrite(5, LOW);
      break;

    case 3:
      digitalWrite(2, LOW);
      digitalWrite(3, HIGH);
      digitalWrite(4, LOW);
      digitalWrite(5, LOW);
      break;

    default:
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      digitalWrite(5, LOW);
      break;
  }
}
