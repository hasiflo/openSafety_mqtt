##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file errorhandler.py
# Functionality for error handling and logging
#

# ##############################################################################
# Copyright (c) 2015, Bernecker+Rainer Industrie-Elektronik Ges.m.b.H. (B&R)
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#    * Neither the name of the copyright holders nor the
#      names of its contributors may be used to endorse or promote products
#      derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# ##############################################################################

# python standard packages or modules
import logging
import sys

# third party packages or modules

# own packages or modules
from pysodb.core import common


##
# \brief Returns appropriate logging level according to the value of verbose
#
# \param verbose Verbose flag (True / False)
#
# \return Verbosity level for logging module
def get_level(verbose):
    if verbose:
        return logging.DEBUG
    else:
        return logging.INFO


##
# \brief Function decorator for system exit
#
# The program will exit after the execution of the decorated function
#
# \param func Function to decorate
#
# \return Decorated function
def causes_systemexit(func):
    def func_wrapper(*args, **kwargs):
        func(*args, **kwargs)
        sys.exit(common.SODBsysexit.EXIT_FAILURE)
        #return func
    return func_wrapper


##
# \brief Wrapping class which wraps some of the functionality of logging.logger
class ErrorHandlerBase(object):

    logger = logging.getLogger(common.SODBinfo.LOGGER_NAME)
    logger.setLevel(logging.DEBUG)

    ##
    # \brief Adds a console output to the errorhandler / logger
    #
    # \param lvl Minimum level of the event for console output
    # \return Handle of the added logging output
    @classmethod
    def add_console_output(cls, lvl):
        consolelogginghandle = logging.StreamHandler()
        consolelogginghandle.setLevel(lvl)
        cls.logger.addHandler(consolelogginghandle)
        return consolelogginghandle

    ##
    # \brief Adds a file output to the errorhandler / logger
    #
    # \param lvl Minimum level of the event for log file output
    # \param filename File name of the log file
    # \return Handle of the added logging output
    @classmethod
    def add_file_output(cls, lvl, filename):
        filelogginghandle = logging.FileHandler(filename)
        filelogginghandle.setLevel(lvl)
        cls.logger.addHandler(filelogginghandle)
        return filelogginghandle

    ##
    #\brief Wrapped function of Python's logging.Logger class
    @classmethod
    def debug(cls, msg, *args, **kwargs):
        cls.logger.debug(msg, *args, **kwargs)

    ##
    #\brief Wrapped function of Python's logging.Logger class
    @classmethod
    def info(cls, msg, *args, **kwargs):
        cls.logger.info(msg, *args, **kwargs)

    ##
    #\brief Wrapped function of Python's logging.Logger class
    @classmethod
    def warning(cls, msg, *args, **kwargs):
        cls.logger.warning(msg, *args, **kwargs)

    ##
    #\brief Wrapped function of Python's logging.Logger class
    @classmethod
    @causes_systemexit
    def error(cls, msg, *args, **kwargs):
        cls.logger.error(msg, *args, **kwargs)

    ##
    #\brief Wrapped function of Python's logging.Logger class
    @classmethod
    @causes_systemexit
    def critical(cls, msg, *args, **kwargs):
        cls.logger.critical(msg, *args, **kwargs)

    ##
    #\brief Wrapped function of Python's logging.Logger class
    @classmethod
    def log(cls, lvl, msg, *args, **kwargs):
        cls.logger.log(lvl, msg, *args, **kwargs)

    ##
    #\brief Wrapped function of Python's logging.Logger class
    @classmethod
    def exception(cls, msg, *args, **kwargs):
        cls.logger.exception(msg, *args, **kwargs)


##
# \brief Class for printing debug information, errors and log them to a logfile
class ErrorHandler(ErrorHandlerBase):

    ##
    # \brief Prints / logs error message, when a file or directory could not
    # beend found
    #
    # \param file_dir File or directory name / path
    # \param module Modulename in which the error occured
    # \param filename File name in which the error occured
    @classmethod
    def file_dir_notfound_error(cls, file_dir, module='', filename=''):
        message = []
        if filename:
            message.append(filename)
        if module:
            message.append(module)
        message.append('filename / directory "{}" not found!'.format(file_dir))
        cls.error(':'.join(message))

##
# \}
# \}
