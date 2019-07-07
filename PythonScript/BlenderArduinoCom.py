import bge
import mathutils
import math
import warnings
import serial
import serial.tools.list_ports
import struct
from struct import*


def DataCom():
    # To get the controller thats running this python script 
    cont = bge.logic.getCurrentController()
    
    # To get the game object this controller is on:
    own = cont.owner
    
    # To get a sensor linked to this controller, so we can run this script in a loop
    mainloop = cont.sensors["MainLoop"]
    
    # Initialization, run this only in the first loop
    if not 'init' in own:
        own['init'] = 1 #create own['init'], so we know that we have done initialization and we do not run it again
        
        arduino_ports = [
            p.device
            for p in serial.tools.list_ports.comports()
            if 'Arduino' in p.description # This may need to be teaked to match new Arduinos
        ]
        
        # Send Error if no Arduino is detected on tthe COM Ports
        if not arduino_ports:
            raise IOError("No Arduino found")
        # Send Warning if multiple Arduinos is detected (the first one detected will be used)
        if len(arduino_ports) > 1:
            warnings.warn('Multiple Arduinos found - using the first')
        
        # Create a Serial Port that contains Arduino Port to be accessed at the specified baud rate (115200 in this case)
        own['serial_port'] = serial.Serial(arduino_ports[0], 115200)
        # Set the timeout for the Serial Port
        own['serial_port'].timeout = 0.1 # you may want to play with this value, higher value e.g. 1 will slow down everything
        
        # Close the Serial Port and then Open (this must be done because the port may be stucked if not closed properly)
        own['serial_port'].close()
        onw['serial_port'].open()
        
        # Checks if there is any I2C devices found in the Arduino
        # Wait for the serial buffer to have 1 bytes of data
        while own['serial_port'].inWaiting() < 1:
            pass
        
        reply = own['serial_port'].read(1)
        # print(reply) #Uncomment for debugging
        
        # Checks the 
        if reply == b'1':
            print("BNO080 is not found!")
            # Exit the game mode
            gameactu = cont.actuators['Game']
            cont.activate(gameactu)
        elif reply == b'2':
            print("BNO080 is detected!")
        else:
            print("Unknown Communication Error")
            gameactu = cont.actuators['Game']
            cont.activate(gameactu)
            
        # start the loop
        mainloop.usePosPulseMode = True
        
    # Main Loop
    myByte = own['serial_port'].read(1)
    if myByte == b'S': # If data read is the Start of the Quaternion Structure sent from the Arduino
        data = own['serial_port'].read(16) # Read 16 bytes 
        myByte = own['serial_port'].read(1)
        if myByte == b'E': # If data read is the End of the Quaternion Structure sent from the Arduino
            quatData = unpack('<fff', data) # Quaternion Byte Data is unpacked into a tuple with an order of (w, x, y, z)
            #print(quatData) # Uncomment for debugging
    
    # Set the Right Arm Bone's rotation with the extracted quaternion data from the Arduino
    own.channels['Bone'].rotation_quaternion = quatData
    own.update() # Update everytime a rotation is made to see changes

DataCom()