''' GNG1103 Prototype #1
Goal:
- take two lengths and convert to coordinate on 4500x8000 (9:16)
- take list of coordinates and create an image that can be exported

Notes:
- a 9:16 aspect ratio maximizes area used on most phone screens
- this code will be ran each time the pen is lifted from the board
'''

import math
import pygame as pg
import csv

# CONSTANTS (metres)
BOARD_WIDTH = 1.0
BOARD_HEIGHT = BOARD_WIDTH * 16 / 9
SPOOL_DISTANCE = 0.6
SIDE_GAP = 0.0

X_PIXELS = 4500
Y_PIXELS = 8000

testA = 1 - SIDE_GAP
testB = 1 - SIDE_GAP

# FUNCTIONS

def get_coordinate(lengths):
    '''(float, float) -> (int, int)
    takes two lengths and returns a tuple with the corresponding x and y coordinates'''
    a = lengths[0]
    b = lengths[1]

    ab_angle = (math.pow(SPOOL_DISTANCE,2)-math.pow(a,2)-math.pow(b,2)) / (-2*a*b)
    if ab_angle>=1 or ab_angle<=-1: #THIS IS SUPER SKETCH AND ONLY FOR TESTING BECAUSE I USED SOME RANDOM COORDINATES
        ab_angle = 0.5
    x_length = a * b * math.sin(ab_angle) / SPOOL_DISTANCE

    b_angle = (b*math.sin(ab_angle)/SPOOL_DISTANCE)
    if b_angle>=1 or b_angle<=-1: #THIS IS ALSO SUPER SKETCH! FIX THIS LATER! FIGURE OUT HOW TO JUST USE LAST POINT OR TO IGNORE POINTS NOT IN DRAWING AREA
        b_angle = 0.5
    y_length = BOARD_HEIGHT - ( (BOARD_HEIGHT-SPOOL_DISTANCE) / 2 + a*math.cos(math.asin(b_angle)) )

    x_coord = round(x_length / BOARD_WIDTH * X_PIXELS)
    y_coord = round(y_length / BOARD_HEIGHT * Y_PIXELS)

    return((x_coord,y_coord))

# MAIN

# setup
white = (255,255,255)
black = (0,0,0)
pen_thickness = 30
canvas = pg.display.set_mode((X_PIXELS,Y_PIXELS))
canvas.fill(white)
file_name = "next_mark.jpg"

# getting lengths and coordinates
with open('testcoordinates.txt') as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=',')
    line_count = 0
    raw_lengths = []
    sine_curve = []
    for row in csv_reader:
        raw_lengths.append((float(row[0])+0.3,float(row[1])+0.3))
        sine_curve.append((round((float(row[0])*1200)),round(float(row[1])*3000)))
        # sine curve to demonstrate how the drawing will work
        line_count += 1
#print(raw_lengths)
coordinates = []
for raw_val in raw_lengths:
    coordinates.append(get_coordinate(raw_val))
#print(coordinates)

# drawing
pg.draw.rect(canvas, black, (10,10,4480,7980), 50)
pg.draw.lines(canvas, black, False, coordinates, pen_thickness)
pg.draw.lines(canvas, black, False, sine_curve, pen_thickness)

# save and display
pg.image.save(canvas,file_name)

pg.display.flip()
# event loop and exit conditions
# (press escape key or click window title bar x to exit)
while True:
    for event in pg.event.get():
        if event.type == pg.QUIT:
            # most reliable exit on x click
            pg.quit()
            raise SystemExit
        elif event.type == pg.KEYDOWN:
            # optional exit with escape key
            if event.key == pg.K_ESCAPE:
                pg.quit()
                raise SystemExit
