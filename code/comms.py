# Copyright 2022 Raquel Gould and Abi Sullivan. All Rights Reserved.
#
# Licensed under ...
#

import logging


def initialize():
  logging.debug('comms initializing...')
  
  logging.info('comms initialized.')
  return True


def shutdown():
  logging.debug('comms shutting down...')
  
  logging.info('comms shut down.')
