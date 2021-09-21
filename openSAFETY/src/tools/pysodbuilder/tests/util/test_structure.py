##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_structure.py
# Unit test for the module pysodb.util.structure

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

from pysodb.util.structure import Structmember, Struct, SPDOStruct, \
    SOD_ActLenPtrData


##
# \brief Tests the class Structmember
class Test_Structmember(unittest.TestCase):

    ##
    # \brief Tests object initialisation, expected attributes and values
    def test_object_initialisation(self):

        expected_attributes = {
            'datatype' : 1,
            'identifier' : 2,
            'comment' : 3,
            'initval' : 4,
            'arraysize' : 5
             }

        expected_attributes2 = {
            'datatype' : 1,
            'identifier' : 2,
            'comment' : 3,
            'initval' : None,
            'arraysize' : None
             }

        expected_attributes3 = {
            'datatype' : 1,
            'identifier' : 2,
            'comment' : 3,
            'initval' : 4,
            'arraysize' : None
             }

        obj = Structmember(1, 2, 3, 4, 5)
        check_expected_attributes(self, obj, expected_attributes)

        obj = Structmember(1, 2, 3)
        check_expected_attributes(self, obj, expected_attributes2)

        obj = Structmember(1, 2, 3, 4)
        check_expected_attributes(self, obj, expected_attributes3)


##
# \brief Tests the class Struct
class Test_Struct(unittest.TestCase):

    ##
    # \brief Tests object initialisation, expected attributes and their values
    def test_object_initialisation(self):

        expected_attributes = {
            'datatype' : 1,
            'members' : [],
            'identifier' : 3,
            'comment' : 4
            }

        expected_attributes2 = {
            'datatype' : 1,
            'members' : [],
            'identifier' : 3,
            'comment' : None
            }

        expected_attributes3 = {
            'datatype' : 1,
            'members' : [],
            'identifier' : None,
            'comment' : None
            }

        structmembers = [Structmember('1', 'm1', None, 4),
                         Structmember('2', 'm2', None, 5),
                         Structmember('3', 'm3', None, 6)]


        expected_attributes4 = {
            'datatype' : 1,
            'members' : structmembers[:],
            'identifier' : None,
            'comment' : None
            }

        obj = Struct(1, [], 3, 4)
        check_expected_attributes(self, obj, expected_attributes)

        obj = Struct(1, [], 3)
        check_expected_attributes(self, obj, expected_attributes2)

        obj = Struct(1, [])
        check_expected_attributes(self, obj, expected_attributes3)

        obj = Struct(1, structmembers)
        check_expected_attributes(self, obj, expected_attributes4)

        self.assertEqual(obj.initval, [4, 5, 6])

    ##
    # \brief Tests the method refresh_initvalues() and expected results
    def test_refresh_initvalues(self):

        structmembers = [Structmember('1', 'm1', None, 11),
                         Structmember('2', 'm2', None, 10),
                         Structmember('3', 'm3', None, 12)]

        obj = Struct('type', structmembers)
        self.assertEqual(obj.initval, [11, 10, 12])

        obj.members.append(Structmember('4', 'm4', None, 'otherinitvalue'))
        obj.refresh_initvalues()
        self.assertEqual(obj.initval, [11, 10, 12, 'otherinitvalue'])


##
# \brief Tests the class SPDOStruct
class Test_SPDOStruct(unittest.TestCase):

    ##
    # \brief Tests object initialisation, expected attributes and values
    def test_object_initialisation(self):
        expected_attributes = {
            'datatype' : 'tSPDOTransportSafeIN',
            'members' : [],
            'identifier' : 'identifier',
            'comment' : 'acomment'
            }

        expected_attributes2 = {
            'datatype' : 'tSPDOTransportSafeIN',
            'members' : [],
            'identifier' : 'transpSafeIN_g',
            'comment' : None
            }

        structmembers = [Structmember('1', 'm1', None, 4),
                         Structmember('2', 'm2', None, 5),
                         Structmember('3', 'm3', None, 6)]

        expected_attributes3 = {
            'datatype' : 'tSPDOTransportSafeIN',
            'members' : structmembers[:],
            'identifier' : 'transpSafeIN_g',
            'comment' : None
            }

        obj = SPDOStruct('SafeIN', [], 'identifier', 'acomment')
        check_expected_attributes(self, obj, expected_attributes)

        obj = SPDOStruct('SafeIN', [])
        check_expected_attributes(self, obj, expected_attributes2)

        obj = obj = SPDOStruct('SafeIN', structmembers[:])
        check_expected_attributes(self, obj, expected_attributes3)


##
# \brief Tests the class SOD_ActLenPtrData
class Test_SOD_ActLenPtrData(unittest.TestCase):

    ##
    # \brief Tests object initialisation, expected attributes and values
    def test_object_initialisation(self):

        expected_attributes = {
            'datatype' : 'SOD_t_ACT_LEN_PTR_DATA',
            'members' : [],
            'identifier' : 'ptrdata_identifier',
            'comment' : None,
            'ptr_data' : 'ptrdata_ptrdata',
            'act_len' : 'ptrdata_actlen',
            'sektor' : 'ptrdata_sektor'
            }

        obj = SOD_ActLenPtrData('ptrdata_ptrdata',
                                'ptrdata_actlen',
                                'ptrdata_identifier',
                                'ptrdata_sektor')

        for item in expected_attributes.items():
            key = item [0]
            self.assertTrue(hasattr(obj, key))

        self.assertTrue(isinstance(obj.act_len, Structmember))
        self.assertTrue(hasattr(obj.act_len, 'identifier'))
        self.assertEqual(getattr(obj.act_len, 'identifier'), 'dw_actLen')

        self.assertTrue(isinstance(obj.ptr_data, Structmember))
        self.assertTrue(hasattr(obj.ptr_data, 'identifier'))
        self.assertEqual(getattr(obj.ptr_data, 'identifier'), 'pv_objData')

if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
