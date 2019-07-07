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

// Intitialize Global Variables
float quat[4] = {1.0f, 0.0f, 0.0f, 0.0f}; //Float array that holds the quaternion values
int data_length = sizeof(quat); //Length of the quaternion array in Bytes (should be 16 bytes)

#include <Wire.h>

#include "SparkFun_BNO080_Arduino_Library.h"
BNO080 myIMU;

void setup()
{
  Serial.begin(115200);

  Wire.begin();

  if (myIMU.begin() == false)
  {
    // Serial.println("BNO080 not detected at default I2C address. Check your jumpers and the hookup guide. Freezing...");
    Serial.write('1'); // Send a '1' to indicate
    while (1);
  }

  // Send a byte for python to determine if BNO080 is detected 
  Serial.write('2'); // Send a '2' to indicate

  Wire.setClock(400000); //Increase I2C data rate to 400kHz

  myIMU.enableRotationVector(50); //Send data update every 50ms

  //Serial.println(F("Rotation vector enabled"));
  //Serial.println(F("Output in form i, j, k, real, accuracy"));
}

void loop()
{
  //Look for reports from the IMU
  if (myIMU.dataAvailable() == true)
  {
    quat[1] = myIMU.getQuatI();
    quat[2] = myIMU.getQuatJ();
    quat[3] = myIMU.getQuatK();
    quat[0] = myIMU.getQuatReal();
    float quatRadianAccuracy = myIMU.getQuatRadianAccuracy();

    
    /* //Uncomment to view real time quaternion data for debugging 
    Serial.print(quat[0], 2);
    Serial.print(F(","));
    Serial.print(quat[1], 2);
    Serial.print(F(","));
    Serial.print(quat[2], 2);
    Serial.print(F(","));
    Serial.print(quat[3], 2);
    Serial.print(F(","));
    Serial.print(quatRadianAccuracy, 2);
    Serial.print(F(","));

    Serial.println();
    */

    // Send the quaternion data
    // Wrap the quaternion structure with a Start("S") Byte and an End("E") Byte to make sure a full structure is received 
    Serial.write('S');
    Serial.write((uint8_t *)&quat, data_length);
    Serial.write('E');
    
  }
}
