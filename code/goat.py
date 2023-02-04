# Copyright 2022 Raquel Gould and Abi Sullivan. All Rights Reserved.
#
# Licensed under ...
#

import sys
import time
import logging
import RPi.GPIO as GPIO
import os

import detect
import comms
import arm
import motion

codeDirectory = '/home/pi/Documents/goat/code/'

logging.basicConfig(format='%(asctime)s | %(levelname)s | %(module)s | %(funcName)s | %(message)s', level=logging.INFO)


def initialize():
    global codeDirectory
    logging.debug('GOAT initializing...')

    detect.codeDirectory = codeDirectory
#     if detect.initialize():
#         detect.detectTrash()
#     else:
#         logging.critical('detect failed to initialize.')
#         sys.exit('Initialization failure')

    if not comms.initialize():
        logging.critical('comms failed to initialize.')
        sys.exit('Initialization failure')

    arm.codeDirectory = codeDirectory
    if not arm.initialize():
        logging.critical('arm failed to initialize.')
        sys.exit('Initialization failure')
    
    if not motion.initialize():
        logging.critical('motion failed to initialize.')
        sys.exit('Initialization failure')
    
    logging.info('GOAT initialized.')


def shutdown():
    logging.debug('GOAT shuting down...')
    motion.shutdown()
    arm.shutdown()
    comms.shutdown()
    detect.shutdown()
    logging.debug('GOAT shutdown complete.')


def runGoat():
    arm.armUp()
    
    while True:
        detect.initialize()
        
        # Don't know why yet, but we seem to have to run detectTrash 5 times for it to work well
        logging.info('detectTrash 1')
        detectionResult = detect.detectTrash()
        logging.info('detectTrash 2')
        detectionResult = detect.detectTrash()
        logging.info('detectTrash 3')
        detectionResult = detect.detectTrash()
        logging.info('detectTrash 4')
        detectionResult = detect.detectTrash()
        logging.info('detectTrash 5')
        detectionResult = detect.detectTrash()
        
        for detection in detectionResult.detections:
            # Find the centerpoint in pixels (ni!)
            bbox = detection.bounding_box
            
            debugString = 'x = ' + str(bbox.origin_x) + ' width = ' + str(bbox.width) + ' y = ' + str(bbox.origin_y) + ' height = ' + str(bbox.height)
            logging.info(debugString)
            
            trashX = (bbox.origin_x + bbox.width + bbox.origin_x) / 2
            trashY = (bbox.origin_y + bbox.width + bbox.origin_y) / 2
            
            # Convert from pixels to centimeters
            trashX = trashX * 52.5 / 583
            trashY = trashY * 30 / 330
            
            # Convert to GOAT cooderinates
            trashX = trashX - 52.5 / 2
            trashY = 35 - trashY

            arm.move(trashX, trashY, 0)
            arm.grabTpRoll()
            arm.dump()
            arm.armUp()
        
        detect.shutdown()
        
        # Check if switch was turned off
        global shutdownPin
        if GPIO.input(shutdownPin):
            return
        
def main():
    global shutdownPin
    try:
        logging.info('GOAT starting up...')
        
        # Wait for switch to be turned on
        shutdownPin = 23
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(shutdownPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
        while GPIO.input(shutdownPin):
            time.sleep(1)
            
        initialize()
        runGoat()

    finally:
        shutdown()
        
        # Check if we should shutdown the Raspberry Pi
        if GPIO.input(shutdownPin):
            os.system('sudo systemctl poweroff') 

        GPIO.cleanup()

if __name__ == '__main__':
    main()
