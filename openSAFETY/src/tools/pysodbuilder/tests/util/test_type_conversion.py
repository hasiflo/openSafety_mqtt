##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_type_conversion.py
# Unit test for the module pysodb.util.type_conversion

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
from pysodb.util.sod_data import SODDataTypes
from pysodb.util.type_conversion import SODDataTypes_ObjectLength_CLang, \
    OSDDValueType_TargetDataType, SODDataType_TargetDataType, \
    OSDDValueType_SODDataType, SODDataType_SODRangeDataType, \
    SODDataType_DataWord, SODDataType_DefaultValue_Prefix, \
    SODDataType_ActData_Prefix


##
#\brief Tests the class SODDataTypes_ObjectLength_CLang
class Test_SODDataTypes_ObjectLength_CLang(unittest.TestCase):

    expected_values = [
         ('U8', '0x1UL'),
         ('U16', '0x2UL'),
         ('U32', '0x4UL'),

         ('I8', '0x1UL'),
         ('I16', '0x2UL'),
         ('I32', '0x4UL'),

         ('EPLS_k_BOOLEAN', '0x01UL')
         ]

    ##
    # \brief Tests the class for expected attributes and their values
    def test_attributes(self):
        self.assertTrue(hasattr(SODDataTypes_ObjectLength_CLang, '_list'))
        self.assertEqual(
            SODDataTypes_ObjectLength_CLang._list, self.expected_values)

    ##
    # \brief Tests the method get_ObjectLength() for expected values
    def test_get_ObjectLength(self):
        for item in self.expected_values:
            value = SODDataTypes_ObjectLength_CLang.get_ObjectLength(item[0])
            self.assertEqual(value, item[1])

        value = SODDataTypes_ObjectLength_CLang.get_ObjectLength('invalid')
        self.assertEqual(value, None)


##
# \brief Tests the class OSDDValueType_TargetDataType
class Test_OSDDValueType_TargetDataType(unittest.TestCase):

    expected_values = [
             ('UInt8', 'UINT8'),
             ('UInt16', 'UINT16'),
             ('UInt32', 'UINT32'),

             ('Int8', 'INT8'),
             ('Int16', 'INT16'),
             ('Int32', 'INT32')
         ]

    ##
    # \brief Tests expected attributes and values of the class
    def test_attributes(self):
        self.assertTrue(hasattr(OSDDValueType_TargetDataType, '_list'))
        self.assertEqual(
            OSDDValueType_TargetDataType._list, self.expected_values)

    ##
    # \brief Tests the mehtod get_TargetDataType() for expected values
    def test_get_TargetDataType(self):
        for item in self.expected_values:
            value = OSDDValueType_TargetDataType.get_TargetDataType(item[0])
            self.assertEqual(value, item[1])

        value = OSDDValueType_TargetDataType.get_TargetDataType('invalid')
        self.assertEqual(value, None)


##
# \brief Tests the class SODDataType_TargetDataType
class Test_SODDataType_TargetDataType(unittest.TestCase):

    expected_values = [
         ('U8', 'UINT8'),
         ('U16', 'UINT16'),
         ('U32', 'UINT32'),

         ('I8', 'INT8'),
         ('I16', 'INT16'),
         ('I32', 'INT32')
     ]

    ##
    # \brief Tests the class for expected attributes and their values
    def test_attributes(self):
        self.assertTrue(hasattr(SODDataType_TargetDataType, '_list'))
        self.assertEqual(SODDataType_TargetDataType._list, self.expected_values)

    ##
    # \brief Test the method get_TargetDataType() for expected values
    def test_get_TargetDataType(self):
        for item in self.expected_values:
            value = SODDataType_TargetDataType.get_TargetDataType(item[0])
            self.assertEqual(value, item[1])

        value = SODDataType_TargetDataType.get_TargetDataType('invalid')
        self.assertEqual(value, None)


##
# \brief Tests the class OSDDValueType_SODDataType
class Test_OSDDValueType_SODDataType(unittest.TestCase):

    expected_values = [
         ('UInt8', 'U8'),
         ('UInt16', 'U16'),
         ('UInt32', 'U32'),

         ('Int8', 'I8'),
         ('Int16', 'I16'),
         ('Int32', 'I32')
     ]

    ##
    # \brief Tests the calss for expected attributes and their values
    def test_attributes(self):
        self.assertTrue(hasattr(OSDDValueType_SODDataType, '_list'))
        self.assertEqual(OSDDValueType_SODDataType._list, self.expected_values)

    ##
    # \brief Tests the method () for expected values
    def test_get_SODDataType(self):
        for item in self.expected_values:
            value = OSDDValueType_SODDataType.get_SODDataType(item[0])
            self.assertEqual(value, item[1])

        value = OSDDValueType_SODDataType.get_SODDataType('invalid')
        self.assertEqual(value, None)


##
# \brief Tests the class SODDataType_SODRangeDataType
class Test_SODDataType_SODRangeDataType(unittest.TestCase):

    expected_values = [
         (['I8', 'U8' ], 'U8'),
         (['I16', 'U16'], 'U16'),
         (['I32', 'U32'], 'U32')
     ]

    expected_values2 = [SODDataTypes.U8, SODDataTypes.U16, SODDataTypes.U32]

    ##
    # \brief Tests the class for expected attributes and their values
    def test_attributes(self):
        self.assertTrue(hasattr(SODDataType_SODRangeDataType, '_list'))
        self.assertEqual(
            SODDataType_SODRangeDataType._list, self.expected_values)

    ##
    # \brief Tests the method get_SODRangeDataType() for expected values
    def test_get_SODRangeDataType(self):
        for item in self.expected_values:
            for listitem in item[0]:
                value = \
                    SODDataType_SODRangeDataType.get_SODRangeDataType(listitem)
                self.assertEqual(value, item[1])

        value = SODDataType_SODRangeDataType.get_SODRangeDataType('invalid')
        self.assertEqual(value, None)

    ##
    # \brief Tests the mehtod get_SODRangeDataTypes() for expected values
    def test_get_SODRangeDataTypes(self):
        value = SODDataType_SODRangeDataType.get_SODRangeDataTypes()
        self.assertEqual(value, self.expected_values2)


##
# \brief Tests the class SODDataType_DataWord
class Test_SODDataType_DataWord(unittest.TestCase):
    expected_values = [
         (['U8', 'I8'], 'b'),
         (['U16', 'I16'], 'w'),
         (['U32', 'I32'], 'dw')
     ]

    ##
    # \brief Tests the class for expected attributes and their values
    def test_attributes(self):
        self.assertTrue(hasattr(SODDataType_DataWord, '_list'))
        self.assertEqual(SODDataType_DataWord._list, self.expected_values)

    ##
    # \brief Tests the method get_DataWord_string() for expected values
    def test_get_DataWord_string(self):
        for item in self.expected_values:
            for listitem in item[0]:
                value = SODDataType_DataWord.get_DataWord_string(listitem)
                self.assertEqual(value, item[1])

        value = SODDataType_DataWord.get_DataWord_string('invalid')
        self.assertEqual(value, None)


##
# \brief Tests the class SODDataType_DefaultValue_Prefix
class Test_SODDataType_DefaultValue_Prefix(unittest.TestCase):

    expected_values = [
         (['U8', 'I8'], 'b'),
         (['U16', 'I16'], 'w'),
         (['U32', 'I32'], 'dw'),
         (['OCT'], 'ab'),
         (['DOM'], 's')
     ]

    ##
    # \brief Tests the class for expected values and their default values
    def test_attributes(self):
        self.assertTrue(hasattr(SODDataType_DefaultValue_Prefix, '_list'))
        self.assertEqual(
            SODDataType_DefaultValue_Prefix._list, self.expected_values)

    ##
    # \brief Tests the method get_DataWord_string() for expected values
    def test_get_DataWord_string(self):
        for item in self.expected_values:
            for listitem in item[0]:
                value = \
                    SODDataType_DefaultValue_Prefix.get_DataWord_string(listitem)
                self.assertEqual(value, item[1])

        value = SODDataType_DefaultValue_Prefix.get_DataWord_string('invalid')
        self.assertEqual(value, None)


##
# \brief Tests the class SODDataType_ActData_Prefix
class Test_SODDataType_ActData_Prefix(unittest.TestCase):
    expected_values = [
         (['U8', 'I8'], 'b'),
         (['U16', 'I16'], 'w'),
         (['U32', 'I32'], 'dw'),
         (['OCT', 'DOM'], 'ab')
     ]

    ##
    # \brief Tests the class for expected attributes and their values
    def test_attributes(self):
        self.assertTrue(hasattr(SODDataType_ActData_Prefix, '_list'))
        self.assertEqual(SODDataType_ActData_Prefix._list, self.expected_values)

    ##
    # \brief Tests the method get_DataWord_string() for expected values
    def test_get_DataWord_string(self):
        for item in self.expected_values:
            for listitem in item[0]:
                value = SODDataType_ActData_Prefix.get_DataWord_string(listitem)
                self.assertEqual(value, item[1])

        value = SODDataType_ActData_Prefix.get_DataWord_string('invalid')
        self.assertEqual(value, None)

if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
