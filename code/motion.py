# Copyright 2022 Raquel Gould and Abi Sullivan. All Rights Reserved.
#
# Licensed under ...
#

import logging


def initialize():
    logging.debug('motion initializing...')

    logging.info('motion initialized.')
    return True


def shutdown():
    logging.debug('motion shutting down...')

    logging.info('motion shut down complete.')
