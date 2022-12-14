# Copyright 2022 Raquel Gould and Abi Sullivan. All Rights Reserved.
#
# Licensed under ...
#

import sys
# import time
import logging

import detect
import comms
import arm
import motion

logging.basicConfig(format='%(asctime)s | %(levelname)s | %(module)s | %(funcName)s | %(message)s', level=logging.DEBUG)


def initialize():
    logging.debug('GOAT initializing...')
    if not detect.initialize():
        logging.critical('detect failed to initialize.')
        sys.exit('Initialization failure')
    if not comms.initialize():
        logging.critical('comms failed to initialize.')
        sys.exit('Initialization failure')
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
    while True:
        detectionResult = detect.detectTrash()
    

def main():
    try:
        initialize()
        runGoat()

    finally:
        shutdown()

if __name__ == '__main__':
    main()
