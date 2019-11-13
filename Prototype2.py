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

# CONSTANTS
white = (255,255,255)
black = (0,0,0)
pen_thickness = 30
file_name = "whiteboard.jpg"

# FUNCTIONS
def draw_current():
    '''() -> (list of tuples)
    Reads file of string lengths and adds current pen stroke to already existing canvas
    Saves the file'''
    with open('lengths.txt') as file:
        reader = csv.reader(file, delimiter = ',')
        coordinates = []
        for row in reader:
            coordinates.append(Prototype1.get_coordinate((float(row[0]),float(row[1]))))
    return(coordinates)

def get_email():
    '''() -> (String)
    Reads DashboardInfo.txt and returns email inside
    '''
    with open('DashboardInfo.txt') as file:
        reader = csv.reader(file, delimiter = ',')
    return(reader[0][1])

# MAIN
canvas = pg.display.set_mode((Prototype1.X_PIXELS,Prototype1.Y_PIXELS))
canvas.fill(white)

for i in range(0,4): #change this loop to while true when actually using the device
    pg.draw.lines(canvas, black, False, draw_current(), pen_thickness)
    pg.image.save(canvas,file_name)

    dash_file = open('DashboardInfo.txt', 'r')
    dash_info = []
    for i in range (0,3):
        dash_info.append(dash_file.readline())
        #0 for dont send, 1 for send
        #email to send to
        #clear screen?
    dash_file.close()

    print('1' in dash_info[0])

    if '1' in dash_info[0]:
        emailer.send_email(dash_info[1])
    if '1' in dash_info[2]:
        canvas.fill(white)
        pg.image.save(canvas,file_name)
        emailer.send_email(dash_info[1])
        
    time.sleep(10) # use shorter time for real use
