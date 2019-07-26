# ArduinoMotionCapture
This project extracts the quaternion data from a BNO080 via Arduino and stream the data to a Blender 3D Model for a real time visual feedback.

## Getting Started

To run this program, the following software are needed:

- [Blender](https://www.blender.org/download/releases/2-79/) [Ver 2.79]

- [Arduino](https://www.arduino.cc/en/main/software) [Ver 1.8.9]: this software is need to change the code of the Arduino.

## Programs
### BNO080_StreamQuatData.ino
This is the main Arduino program that will read the quaternion data from the IMU BNO080 and stream the data to Blender.

### MoCapBody.blend
This the blender file that contains the 3D Model that will be oriented by the received quaternion data from the Arduino. It also makes use of the BGE (Blender Game Engine) Logic to be able to continuously update the orentation of the 3D model in real time. 

### BlenderArduinoCom.py
This is the python script that the Blender file will be using to read the quaternion data from the Arduino and update the orientation of the Blender model with the received data.



## Libraries
- [PySerial](https://pypi.org/project/pyserial/) [Version 3.4]
  - This library is used to establish a serial connection between python and arduino to be able to read and write data between the two systems.
  
- [SparkFun BNO080 Arduino Library](https://github.com/sparkfun/SparkFun_BNO080_Arduino_Library) 
  - This library is used to extract the quaternion data from the IMU BNO080