# Copyright 2022-2023 Raquel Gould and Abi Sullivan. All Rights Reserved.
#

import comms
import logging

import ikpy.chain
import ikpy.utils.plot as plot_utils
import matplotlib.pyplot as plt

import numpy as np
import math

import ipywidgets as widgets

codeDirectory = ''

def initialize():
    logging.debug('arm initializing...')

    global my_chain
    my_chain = ikpy.chain.Chain.from_urdf_file(codeDirectory + "goat_arm.urdf",
                                               active_links_mask=[False, True, True, True, True, True, True])

    global target_orientation
    target_orientation = [0, 1, 0]

    global fig
    global ax
    fig, ax = plot_utils.init_3d_figure()

    # send initialize command to Arduino
    comms.sendCommand('arm', 'initialize', 0)
    
    return True

def doInverseKinematics():
    global inverseKinematics
    global target_position
    global target_orientation
    global armJointPositions
    
#    old_position = inverseKinematics.copy()
    inverseKinematics = my_chain.inverse_kinematics(target_position,
                                                    target_orientation,
                                                    orientation_mode="all") #,
                                                    #initial_position=old_position)
    
    armJointPositions = list(map(lambda r:math.degrees(r),inverseKinematics.tolist()))
    debugString = "The angles of each joints are : ", armJointPositions
    logging.debug(debugString)
    
    computed_position = my_chain.forward_kinematics(inverseKinematics)
    debugString = "Computed position: %s, original position : %s" % (computed_position[:3, 3], target_position)
    logging.debug(debugString)
    debugString = "Computed position (readable) : %s" % [ '%.2f' % elem for elem in computed_position[:3, 3] ]
    logging.debug(debugString)

def updatePlot():
    global fig
    global ax
    
    ax.clear()
    my_chain.plot(inverseKinematics, ax, target=target_position)
    plt.xlim(-65, 65)
    plt.ylim(-65, 65)
    ax.set_zlim(0, 65)
    fig.canvas.draw()
    fig.canvas.flush_events()

def move(x, y, z):
    global target_position
    global armJointPositions

    target_position = [x, y, z]
    doInverseKinematics()
    updatePlot()
    
    # ADD: send move command to Arduino
    # start with the waist
    waistAngle = armJointPositions[1]
    if 0 <= waistAngle <= 90:
        pass
    elif -90 <= waistAngle < 0:
        waistAngle = waistAngle + 180

    shoulderAngle = armJointPositions[2]
    shoulderAngle = abs(shoulderAngle)
    shoulderAngle = 90 - shoulderAngle

    elbowAngle = armJointPositions[3]
    elbowAngle = abs(elbowAngle)
    elbowAngle = elbowAngle + 90
   
    wristAngle = armJointPositions[4]
    wristAngle = abs(wristAngle)
    wristAngle = 90 - wristAngle
    
    comms.sendCommand('arm', 'waist', waistAngle)
    comms.sendCommand('arm', 'wrist', wristAngle)
    comms.sendCommand('arm', 'elbow', elbowAngle)
    comms.sendCommand('arm', 'shoulder', shoulderAngle)
    
def grabTpRoll():
    comms.sendCommand('arm', 'grab_tp_roll', 0)

def grabClose():
    comms.sendCommand('arm', 'grab_close', 0)
    
def grabOpen():
    comms.sendCommand('arm', 'grab_open', 0)
    
def armUp():
    comms.sendCommand('arm', 'up', 0)
    
def dump():
    comms.sendCommand('arm', 'up', 0)
    comms.sendCommand('arm', 'elbow', 0)
    grabOpen()

def shutdown():
    logging.debug('arm shutting down...')
    comms.sendCommand('arm', 'shutdown', 0)
    logging.info('arm shut down complete.')


# for debugging:
if __name__ == '__main__':
    logging.basicConfig(format='%(asctime)s : %(levelname)s : %(module)s : %(funcName)s : %(message)s',
                        level=logging.DEBUG)
    comms.initialize()
    initialize()
    move(-8.5, 28, 0)
    grabTpRoll()
    dump()
    armUp()
    print('breakpoint here')
