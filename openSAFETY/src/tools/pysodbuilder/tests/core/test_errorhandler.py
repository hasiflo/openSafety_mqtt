##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_errorhandler.py
# Unit test for the module pysodb.core.errorhandler

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
import unittest
import logging
import os

# third party packages or modules

# own packages or modules
from tests.common import current_python_version, python34
from pysodb.core.errorhandler import ErrorHandler, causes_systemexit, get_level


@causes_systemexit
def dummy_for_causes_systemexit_decorator (mutable_list):
    mutable_list.append(__name__)


##
# \brief Tests the module pysodb.core.errorhandler
class Test_errorhandler(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        super(Test_errorhandler, cls).setUpClass()
        if current_python_version < python34:
            ErrorHandler.add_console_output(logging.DEBUG)

    ##
    # \brief Tests the methods and SystemExit behaviour of the ErrorHandler
    # class
    def test_functions_systemexit(self):
        ErrorHandler.debug('Debug')
        ErrorHandler.info('Info')
        ErrorHandler.warning('Warning')

        with self.assertRaises(SystemExit):
            ErrorHandler.error('Error')

        with self.assertRaises(SystemExit):
            ErrorHandler.critical('Critical')

        ErrorHandler.log(logging.DEBUG, 'Debug')
        ErrorHandler.log(logging.INFO, 'Info')
        ErrorHandler.log(logging.WARNING, 'Warning')
        ErrorHandler.log(logging.ERROR, 'Error')
        ErrorHandler.log(logging.CRITICAL, 'Critical')

        try:
            raise Exception
        except:
            # try / except because exception() should be called only within
            # an exception handler
            ErrorHandler.exception('Exception')

    ##
    # \brief Tests the causes_systemexit() decorator
    def test_causes_systemexit_decorator(self):
        alist = []
        with self.assertRaises(SystemExit):
            dummy_for_causes_systemexit_decorator(alist)
        self.assertTrue(len(alist) == 1)

    ##
    # \brief Tests the get_level() function
    def test_get_level(self):
        self.assertEqual(get_level(True), logging.DEBUG)
        self.assertEqual(get_level(False), logging.INFO)
        self.assertEqual(get_level(None), logging.INFO)
        self.assertEqual(get_level('verbose'), logging.DEBUG)

    ##
    # \brief Tests adding a console output (handler) to an ErrorHandler
    # class object
    def test_add_console_output(self):
        chandle = ErrorHandler.add_console_output(logging.DEBUG)
        self.assertIsNotNone(chandle)
        chandle.setLevel(logging.DEBUG)
        ErrorHandler.debug('Console output')

        ErrorHandler.logger.removeHandler(chandle)
        chandle.close()

    ##
    # \brief Tests adding a file output (handler) to an ErrorHandler
    # class object
    def test_add_file_outptut(self):
        logfile_path = os.path.normpath(os.path.dirname(
                                            os.path.dirname(__file__)))
        logfile_path = os.path.join(logfile_path, 'testdata/log.txt')
        out_string = 'File output'

        if not os.path.exists(logfile_path):
            fhandle = ErrorHandler.add_file_output(logging.DEBUG, logfile_path)
            self.assertIsNotNone(fhandle)
            fhandle.setLevel(logging.DEBUG)
            ErrorHandler.debug(out_string)
            ErrorHandler.logger.removeHandler(fhandle)
            fhandle.close()

            with open(logfile_path, 'r') as f:
                file_string = f.readline().rstrip()
                self.assertEqual(out_string, file_string)

            self.assertTrue(os.path.exists(logfile_path))
            os.remove(logfile_path)
            self.assertFalse(os.path.exists(logfile_path))
        else:
            self.assertTrue(False, 'Logfile already existed!')

    ##
    # \brief Tests file_dir_notfound() method of the ErrorHandler class
    def test_file_dir_notfound(self):
        with self.assertRaises(SystemExit):
            ErrorHandler.file_dir_notfound_error('File/Dir', 'Module', __file__)

        with self.assertRaises(SystemExit):
            ErrorHandler.file_dir_notfound_error('File/Dir')

        with self.assertRaises(SystemExit):
            ErrorHandler.file_dir_notfound_error('File/Dir', 'Module')

        with self.assertRaises(SystemExit):
            ErrorHandler.file_dir_notfound_error('File/Dir', filename=__file__)

if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
