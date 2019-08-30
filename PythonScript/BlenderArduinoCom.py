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

        #Create a List containing the inverse quaternions of the bones
        BaseQuat = [1.0, 0.0, 0.0, 0.0]
        own['quat_inv'] = [mathutils.Quaternion(BaseQuat),mathutils.Quaternion(BaseQuat), mathutils.Quaternion(BaseQuat)]

		#Create HashMap for to choose a bone depending on number inputted
        own['bone_map'] = { 1: 'Bone', 2: 'Bone.001', 3: 'Bone.004', 4: 'Bone.002', 5: 'Bone.003', 6: 'Bone.005'}
        
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
        own['serial_port'].open()
        
        print("Arduino Connection Established!")
        
        # Checks if there is any I2C devices found in the Arduino
        reply = 0
        
        while reply != 100:
            # Wait for the serial buffer to have 1 bytes of data
            while own['serial_port'].inWaiting() < 1:
                pass
        
            reply = unpack('B', own['serial_port'].read(1))[0]
            # print(reply) #Uncomment for debugging
            
            if reply == 100:
                print("All BNO080 are functioning!")
            elif reply == 0:
                print("The BNO080 listed above are not detected! Program Terminated!")
                # Exit the game mode
                gameactu = cont.actuators['Game']
                cont.activate(gameactu)
            else:
                print(" IMU ", reply, "is not responding!")
           
        # start the loop
        mainloop.usePosPulseMode = True
        
    # Main Loop
    myByte = own['serial_port'].read(1)
    if myByte == b'S': # If data read is the Start of the Quaternion Structure sent from the Arduino
        IMUnumByte = own['serial_port'].read(1)
        data = own['serial_port'].read(16) # Read 16 bytes 
        myByte = own['serial_port'].read(1)
        if myByte == b'E': # If data read is the End of the Quaternion Structure sent from the Arduino
            quatData = unpack('<ffff', data) # Quaternion Byte Data is unpacked into a tuple with an order of (w, x, y, z)
            #print(quatData) # Uncomment for debugging
    
	# Read which IMU data has just been received 
    IMUnum = unpack('B', IMUnumByte)[0]

    # Set the Bone rotation with the extracted quaternion data from the Arduino
    quaternion = mathutils.Quaternion(quatData)
    own['quat_inv'][IMUnum-1] = mathutils.Quaternion(quatData)
    own['quat_inv'][IMUnum-1].invert()
			
    if IMUnum > 1:
        quaternion = own['quat_inv'][IMUnum-2] * quaternion
	
    own.channels[own['bone_map'][IMUnum]].rotation_quaternion = quaternion
    
	# Update everytime a rotation is made to see changes
    own.update() 
    
DataCom()