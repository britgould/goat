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

    logging.info('arm initialized.')
    return True


def moveTest():
    global target_position
    global target_orientation
    target_position = [0, 0, 0.58]
    target_orientation = [-1, 0, 0]

    global ik
    ik = my_chain.inverse_kinematics(target_position, target_orientation, orientation_mode="Y")
    print("The angles of each joints are : ", list(map(lambda r:math.degrees(r),ik.tolist())))

    computed_position = my_chain.forward_kinematics(ik)
    print("Computed position: %s, original position : %s" % (computed_position[:3, 3], target_position))
    print("Computed position (readable) : %s" % [ '%.2f' % elem for elem in computed_position[:3, 3] ])

    global fig
    global ax
    fig, ax = plot_utils.init_3d_figure()
    fig.set_figheight(9)
    fig.set_figwidth(13)
    my_chain.plot(ik, ax, target=target_position)
    plt.xlim(-0.5, 0.5)
    plt.ylim(-0.5, 0.5)
    ax.set_zlim(0, 0.6)
    plt.ion()


def doIK():
    global ik
    global target_position
    global target_orientation
    old_position = ik.copy()
    ik = my_chain.inverse_kinematics(target_position, target_orientation, orientation_mode="Z",
                                     initial_position=old_position)


def updatePlot():
    global fig
    global ax
    
    ax.clear()
    my_chain.plot(ik, ax, target=target_position)
    plt.xlim(-0.5, 0.5)
    plt.ylim(-0.5, 0.5)
    ax.set_zlim(0, 0.6)
    fig.canvas.draw()
    fig.canvas.flush_events()


def move(x, y, z):
    global target_position
    target_position = [x, y, z]
    doIK()
    updatePlot()

#initialize()
#moveTest()

#move(0,0.2,0.3)


def shutdown():
    logging.debug('arm shutting down...')

    logging.info('arm shut down complete.')
