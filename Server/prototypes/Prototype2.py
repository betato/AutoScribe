''' GNG1103 Prototype #2
Goal:
- read text file that contains "reset image" and "email image" information
- integrate email functionality
- fix errors in Prototype #1 and create restrictions for get_coordinate method
- read text file and run code whenever a new version is saved
'''

import emailer
import math
import pygame
import csv
import time
import serial

# Board Constants / Vars
ASPECT_RATIO = 9 / 16
BOARD_WIDTH = 4
BOARD_HEIGHT = BOARD_WIDTH * ASPECT_RATIO
SPOOL_DISTANCE = 0.6
SIDE_GAP = 0.0
X_PIXELS = 4500
Y_PIXELS = int(round(X_PIXELS * ASPECT_RATIO))
lengths = []

# Drawing Constants / Vars
WHITE = (255,255,255)
BLACK = (0,0,0)
PEN_THICKNESS = 5
IMAGE_NAME = "whiteboard.jpg"
LENGTH_COEFF = 0.0009

# Update Constants / Vars
IMAGE_UPDATE = 2 # Minimum time between saving new images when updates occurr
RECIEVE_UPDATE = 2 # Minimum time between clearing or sending emails
image_last_time = time.time()
recieve_last_time = time.time()
image_modified = False
writing = False


# Init
print("Establishing Serial Connection", end='')
ser = serial.Serial('COM6', 2000000, timeout=.1)
print(" - Done")
canvas = pygame.Surface((X_PIXELS, Y_PIXELS))
canvas.fill(WHITE)


def recieve():
    global writing
    line = str(ser.readline())[2:-5:]
    if line == 'START':
        writing = True
        print("Writing Started")
    elif line == 'END':
        writing = False
        print("Writing Complete")
    elif len(line) >  2:
        split = line.split(', ')
        #print(split[0], split[1])
        lengths.append((float(split[0]), float(split[1])))


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

    x_coord = round(x_length / BOARD_WIDTH * X_PIXELS) + (X_PIXELS/10)
    y_coord = round(y_length / BOARD_HEIGHT * Y_PIXELS)

    return((x_coord,y_coord))


def draw_image():
    global image_modified
    print('Updating Image', end='')
    coords = []
    for length in lengths:
        coords.append(get_coordinate((LENGTH_COEFF*float(length[0]),LENGTH_COEFF*float(length[1]))))
    pygame.draw.lines(canvas, BLACK, False, coords, PEN_THICKNESS)
    coords.clear()
    lengths.clear()
    print(' - Done')
    image_modified = True


def check_dashboard():
    email_file = open('email.txt', 'r')
    email_info = email_file.readline()
    email_file.close()

    reset_file = open('reset.txt', 'r')
    reset_info = reset_file.readline()
    reset_file.close()

    if '*' not in email_info:
        emailer.send_email(email_info)
        # overwrite file with '*'
        email_file = open('email.txt', 'w')
        email_file.write('*')
        print("email sent")
    
    if '1' in reset_info:
        canvas.fill(WHITE)
        pygame.image.save(canvas,IMAGE_NAME)
        reset_file = open('reset.txt', 'w')
        reset_file.write('0')
        print("canvas cleared")

# Main update loop
while True:
    # Recieve serial data until pen is released
    while ser.in_waiting or writing:
        if ser.in_waiting:
            recieve()

    if len(lengths) > 1:
        # Update image after pen is released
        draw_image()

    # Check for erase and email commands from dashboard
    current_time = time.time()
    if current_time >= recieve_last_time + RECIEVE_UPDATE:
        recieve_last_time = current_time
        check_dashboard()

    # Create new image if modified at most every IMAGE_UPDATE seconds
    current_time = time.time()
    if current_time >= image_last_time + IMAGE_UPDATE:
        image_last_time = current_time
        if image_modified:
            print('Saving Image', end='')
            pygame.image.save(canvas,IMAGE_NAME)
            print(' - Done')
            image_modified = False
