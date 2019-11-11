''' GNG1103 Prototype #2
Goal:
- read text file that contains "reset image" and "email image" information
- integrate email functionality
- fix errors in Prototype #1 and create restrictions for get_coordinate method
- read text file and run code whenever a new version is saved
'''

import Prototype1
import math
import pygame as pg
import csv

# CONSTANTS
white = (255,255,255)
black = (0,0,0)
pen_thickness = 30
file_name = "whiteboard.jpg"

# FUNCTIONS
def draw_current():
    '''() -> ()
    Reads file of string lengths and adds current pen stroke to already existing canvas
    Saves the file'''
    with open('lengths.txt') as file:
        reader = csv.reader(file, delimiter = ',')
        coordinates = []
        for row in reader:
            coordinates.append(Prototype1.get_coordinate((float(row[0]),float(row[1]))))
    return(coordinates)

# MAIN
canvas = pg.display.set_mode((Prototype1.X_PIXELS,Prototype1.Y_PIXELS))
canvas.fill(white)

pg.draw.lines(canvas, black, False, draw_current(), pen_thickness)
pg.image.save(canvas,file_name)








#########################################
