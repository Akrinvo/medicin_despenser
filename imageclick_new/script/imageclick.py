#!/usr/bin/env python3

import rospy
from std_msgs.msg import String
from geometry_msgs.msg import Point
import serial



ser = serial.Serial('/dev/ttyACM0')
encoder_x = 350000
encoder_z = 21600
frame_x = 440
frame_z = 400



def callback(data):
    print(data)
    x = data.x
    z = data.y
    x_coordinate = (encoder_x/frame_x) * x
    z_coordinate = (encoder_z/frame_z) * z
    x_command = "X M " + str(int(x_coordinate))+'\n'
    z_command = "Z M " + str(int(z_coordinate))+'\n'
    print(z_command)
    ser.write(x_command.encode())
    ser.write(z_command.encode())
    



def pantilt_callback(data):
    print(data)
    if(data.data == "U"):
        command = "Y U\n"
        ser.write(command.encode())
    
    if(data.data == "D"):
        command = "Y D\n"
        ser.write(command.encode())

    
    if(data.data == "R"):
        command = "Y R\n"
        ser.write(command.encode())

        
    if(data.data == "L"):
        command = "Y L\n"
        ser.write(command.encode())

        
    if(data.data == "W"):
        command = "Y W\n"
        ser.write(command.encode())

        


def telescope_callback(data):
    print(data.data)

    command = "T M " + data.data + "\n"
    print(command)
    ser.write(command.encode())








def initiate():

    rospy.init_node("imageClick",anonymous=True)
    sub1 = rospy.Subscriber("mouseClick" , Point , callback)
    sub2 = rospy.Subscriber("keyPress",String,pantilt_callback)
    sub3 = rospy.Subscriber("telescope",String,telescope_callback)






if __name__ == "__main__":

    
    initiate()

    rospy.spin()
    ser.close()
