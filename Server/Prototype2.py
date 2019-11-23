''' GNG1103 Prototype #2
Goal:
- read text file that contains "reset image" and "email image" information
- integrate email functionality
- fix errors in Prototype #1 and create restrictions for get_coordinate method
- read text file and run code whenever a new version is saved
'''

import Prototype1
import emailer
import math
import pygame as pg
import csv
import time
import serial

# CONSTANTS
white = (255,255,255)
black = (0,0,0)
pen_thickness = 5
file_name = "whiteboard.jpg"
length_coeff = 0.002

# MAIN
canvas = pg.Surface((Prototype1.X_PIXELS,Prototype1.Y_PIXELS))
canvas.fill(white)
ser = serial.Serial('COM6', 2000000, timeout=.1) # Establish the connection on a specific port
print("Serial Connected")
lengths = []
last_time = time.time()
writing = False

while True: #change this loop to while true when actually using the device
    
    while ser.in_waiting or writing:
        line = str(ser.readline())[2:-5:]
        if line == 'START':
            writing = True
            print("Writing Started")
        elif line == 'END':
            writing = False
            print("Writing Complete")
        elif len(line) >  2:
            split = line.split(', ')
            print(split[0], split[1])
            lengths.append((float(split[0]), float(split[1])))

    #current_time = time.time()
    #current_time >= last_time + 2:
    if len(lengths) > 1:
        #last_time = current_time
        print('Creating Image')
        coords = []
        for length in lengths:
            coords.append(Prototype1.get_coordinate((length_coeff*float(length[0]),length_coeff*float(length[1]))))
        pg.draw.lines(canvas, black, False, coords, pen_thickness)
        pg.image.save(canvas,file_name)
        coords.clear()
        lengths.clear()
        print('Image Created')

        email_file = open('email.txt', 'r')
        email_info = email_file.readline()
        email_file.close()

        reset_file = open('reset.txt', 'r')
        reset_info = reset_file.readline()
        reset_file.close()

        if '*' not in email_info:
            #emailer.send_email(email_info)
            # overwrite file with '*'
            email_file = open('email.txt', 'w')
            email_file.write('*')
            print("email sent")

        if '1' in reset_info:
            canvas.fill(white)
            pg.image.save(canvas,file_name)
            #emailer.send_email(email_info)
            reset_file = open('reset.txt', 'w')
            reset_file.write('0')
            print("canvas cleared")
