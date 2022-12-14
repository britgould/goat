# Copyright 2022 Raquel Gould and Abi Sullivan. All Rights Reserved.
#
# Licensed under ...
#

import serial
import logging

STUB_COMMS_FOR_DEBUG = False

def initialize():
    logging.debug('comms initializing...')

    if STUB_COMMS_FOR_DEBUG:
        logging.info('comms set to DEBUG mode.')
    else:
        global serialComm

        serialComm = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
        serialComm.reset_input_buffer()

    logging.info('comms initialized.')
    return True

def sendCommand(command, parameter, value):
    if STUB_COMMS_FOR_DEBUG:
        return True
    else:
        global serialComm
        
        if not serialComm.isOpen():
            logging.critical('serialComm is not open')
            return False
        else:
            serialMessage = command + ' ' + parameter + ' ' + str(value) + '\n'
            logging.debug('serialMessage: %s', serialMessage)
            serialComm.write(serialMessage.encode('utf-8'))
        return True  
#             while True:
#                 while serialComm.inWaiting() == 0: pass
#                 if serialComm.inWaiting() > 0:
#                     response = serialComm.readline().decode('utf-8').rstrip()
#                     logging.debug('serial response: %s', response)
#                     if response == 'OK':
#                         return True
#                     else:
#                         logging.info('Arduino says: %s', response)
                
def shutdown():
    logging.debug('comms shutting down...')

    if not STUB_COMMS_FOR_DEBUG:
        global serialComm
        serialComm.close()
        
    logging.info('comms shut down complete.')
