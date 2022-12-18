# Copyright 2022 Raquel Gould and Abi Sullivan. All Rights Reserved.
#
# Licensed under ...
#
# Derived from https://github.com/tensorflow/examples.git by
# The TensorFlow Authors, which is licensed under:
#     http://www.apache.org/licenses/LICENSE-2.0
#

import argparse
import sys
import logging

import cv2
from tflite_support.task import core
from tflite_support.task import processor
from tflite_support.task import vision
import utils


def initialize():
    logging.debug('detect initializing...')

    # Start capturing video input from the camera
    global cap
    cap = cv2.VideoCapture(0)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1920)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 1080)

    # Initialize the object detection model
    base_options = core.BaseOptions(
        file_name='../detection/trash.tflite', use_coral=False, num_threads=4)
    detection_options = processor.DetectionOptions(
        max_results=3, score_threshold=0.3)
    options = vision.ObjectDetectorOptions(
        base_options=base_options, detection_options=detection_options)
    global detector
    detector = vision.ObjectDetector.create_from_options(options)

    logging.info('detect initialized.')
    return True


def detectTrash():
    # Visualization parameters
    row_size = 20  # pixels
    left_margin = 24  # pixels
    text_color = (0, 0, 255)  # red
    font_size = 1
    font_thickness = 1

    # Continuously capture images from the camera and run inference
    global cap
    if cap.isOpened():
        success, image = cap.read()
        if not success:
            logging.critical('Cannot read from webcam.')
            sys.exit('ERROR: Cannot read from webcam.')

        #image = cv2.flip(image, 1)

        # Convert the image from BGR to RGB as required by the TFLite model.
        rgb_image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

        # Create a TensorImage object from the RGB image.
        input_tensor = vision.TensorImage.create_from_array(rgb_image)

        # Run object detection estimation using the model.
        global detector
        detection_result = detector.detect(input_tensor)

        # Draw keypoints and edges on input image
        image = utils.visualize(image, detection_result)

        # Show text on images
        onImageText = 'GOAT Vision'
        textLocation = (left_margin, row_size)
        cv2.putText(image, onImageText, textLocation, cv2.FONT_HERSHEY_PLAIN,
                    font_size, text_color, font_thickness)

        # Stop the program if the ESC key is pressed.
        if cv2.waitKey(1) == 27:
            sys.exit()
        cv2.imshow('object_detector', image)
        
        return detection_result
    
    else:
        logging.critical('Cannot open webcam.')
        sys.exit('ERROR: Cannot open webcam.')
        


def shutdown():
    logging.debug('detect shutting down...')
    global cap
    cap.release()
    cv2.destroyAllWindows()
    logging.info('detect shut down complete.')

