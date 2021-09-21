##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_filehandling.py
# Unit test for the module pysodb.core.filehandling

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
import os

# third party packages or modules

# own packages or modules
from pysodb.core import filehandling


##
# \brief Tests the pysodb.core.filehandling module
class Test_filehandling(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)
        filehandling.FileHandling.encoding = None

    ##
    # \brief Tests setting an encoding
    def test_set_encoding(self):
        used_encoding = 'utf8'
        self.assertIsNone(filehandling.FileHandling.encoding)
        filehandling.FileHandling.set_encoding(used_encoding)
        self.assertTrue(filehandling.FileHandling.encoding == used_encoding)

    ##
    # \brief Tests opening a file with a utf8 encoding
    def test_open_utf8(self):
        used_encoding = 'utf8'
        filehandling.FileHandling.set_encoding(used_encoding)
        testdata_path = \
            os.path.normpath(os.path.dirname(os.path.dirname(__file__)))
        file_path = os.path.join(testdata_path, 'testdata/utf8test.txt')

        test_string = 'Hello World!'.encode(encoding=used_encoding)

        with filehandling.FileHandling.open(file_path, 'rb') as f:
            s = f.read()
            self.assertEqual(s.encode(used_encoding), test_string)

    ##
    # \brief Tests opening a file with utf16 encoding
    def test_open_utf16(self):
        used_encoding = 'utf16'
        filehandling.FileHandling.set_encoding(used_encoding)
        testdata_path = \
             os.path.normpath(os.path.dirname(os.path.dirname(__file__)))
        file_path = os.path.join(testdata_path, 'testdata/utf16test.txt')

        test_string = 'Hello World!'.encode(encoding=used_encoding)

        with filehandling.FileHandling.open(file_path, 'rb') as f:
            s = f.read()
            self.assertEqual(s.encode(used_encoding), test_string)

    ##
    # \brief Tests opening a file with utf32le encoding
    def test_open_utf32le(self):
        used_encoding = 'utf_32_le'
        filehandling.FileHandling.set_encoding(used_encoding)
        testdata_path = \
            os.path.normpath(os.path.dirname(os.path.dirname(__file__)))
        file_path = os.path.join(testdata_path, 'testdata/utf32letest.txt')

        test_string = 'Hello World!'.encode(encoding=used_encoding)

        with filehandling.FileHandling.open(file_path, 'rb') as f:
            s = f.read()
            self.assertEqual(s.encode(used_encoding), test_string)


if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
