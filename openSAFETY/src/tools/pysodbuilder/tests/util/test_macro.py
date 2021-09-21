##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_macro.py
# Unit test for the module pysodb.util.macro

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

# third party packages or modules

# own packages or modules
from tests.common import check_expected_attributes

from pysodb.util.macro import BitAccessMacro


##
# \brief Tests the class BitAccessMacro
class Test_BitAccessMacro(unittest.TestCase):

    expected_attributes = {
        'macroname' : 'aMacroName',
        'variablename' : 'aVariableName',
        'bitmask' : 0x08,
        'cast_to_datatype' : None,
        'index' : 3
        }

    expected_attributes2 = {
        'macroname' : 'aMacroName2',
        'variablename' : 'aVariableName2',
        'bitmask' : 0x00,
        'cast_to_datatype' : 'aDataType',
        'index' : 0
        }

    expected_attributes3 = {
        'macroname' : 'aMacroName3',
        'variablename' : 'aVariableName3',
        'bitmask' : 4,
        'cast_to_datatype' : 'aDataType3',
        'index' : 2
        }

    ##
    # \brief Tests the object initialisation of a BitAccessMacro object
    def test_object_initialisation(self):

        obj = BitAccessMacro('aMacroName', 'aVariableName', 0x08)
        self.assertTrue(obj)
        self.assertTrue(isinstance(obj, BitAccessMacro))
        check_expected_attributes(self, obj, self.expected_attributes)

        obj = BitAccessMacro('aMacroName2', 'aVariableName2',
                             0x00, 'aDataType')
        check_expected_attributes(self, obj, self.expected_attributes2)

        obj = BitAccessMacro('aMacroName3', 'aVariableName3',
                             '0x04', 'aDataType3')
        check_expected_attributes(self, obj, self.expected_attributes3)

if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
