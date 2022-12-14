# Copyright 2022 Raquel Gould and Abi Sullivan. All Rights Reserved.
#
# Licensed under ...
#

import logging

import ikpy.chain
import ikpy.utils.plot as plot_utils
import matplotlib.pyplot as plt

import numpy as np
import time
import math

import ipywidgets as widgets


def initialize():
    logging.debug('arm initializing...')

    global my_chain
    my_chain = ikpy.chain.Chain.from_urdf_file("goat_arm.urdf", active_links_mask=[False, True, True, True, True, True])

    global target_position
    global target_orientation
    target_position = [0, 0, 63]
    target_orientation = [0, 1, 0]

    global inverseKinematics
    inverseKinematics = my_chain.inverse_kinematics(target_position, target_orientation, orientation_mode="Y")
    print("The angles of each joints are : ", list(map(lambda r:math.degrees(r),inverseKinematics.tolist())))

    computed_position = my_chain.forward_kinematics(inverseKinematics)
    print("Computed position: %s, original position : %s" % (computed_position[:3, 3], target_position))
    print("Computed position (readable) : %s" % [ '%.2f' % elem for elem in computed_position[:3, 3] ])

    global fig
    global ax
    fig, ax = plot_utils.init_3d_figure()
#    fig.set_figheight(9)
#    fig.set_figwidth(13)
    my_chain.plot(inverseKinematics, ax, target=target_position)
    plt.xlim(-65, 65)
    plt.ylim(-65, 65)
    ax.set_zlim(0, 65)
    plt.ion()

    logging.info('arm initialized.')
    return True

def doInverseKinematics():
    global inverseKinematics
    global target_position
    global target_orientation
    old_position = inverseKinematics.copy()
    inverseKinematics = my_chain.inverse_kinematics(target_position, target_orientation, orientation_mode="Z",
                                     initial_position=old_position)


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
    target_position = [x, y, z]
    doInverseKinematics()
    updatePlot()

def shutdown():
    logging.debug('arm shutting down...')

    logging.info('arm shut down complete.')

initialize()

print('one')
#input('proceed?')

move(0,63,13)

#input('proceed?')
print('two')
