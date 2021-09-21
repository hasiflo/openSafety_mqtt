##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_osdd_data.py
# Unit test for the module pysodb.util.osdd_data

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
from tests.common import check_expected_attributes, DummyOSDDSetting

from pysodb.util.osdd_data import Channel, ChannelGroup, Setting, \
    SettingsGroup, Function, SettingDataTemplate, Transport, SPDOTransport, \
    OSDDSettingDataTemplateElements, OSDDVisibility, OSDDDirections, \
    OSDDValueType, OSDDValueTypes, get_sorted_settings_list


##
# \brief Tests the Channel class
class Test_Channel(unittest.TestCase):

    expected_attributes = {
        'id' : 1,
        'datatype' : 2,
        'transportname' : 3,
        'transportindex' : 4,
        'transportbitmask' : 5,
        'sodindex' : 6,
        'sodsubindex' : 7,
        'activeinfunctions' : 8,
        'settings' : 9
        }

    ##
    # \brief Tests the object initialisation of a Channel object
    def test_object_initialisation(self):

        obj = Channel(1, 2, 3, 4, 5, 6, 7, 8, 9)
        self.assertTrue(isinstance(obj, Channel))
        check_expected_attributes(self, obj, self.expected_attributes)


##
# \brief Tests the ChannelGroup class
class Test_ChannelGroup(unittest.TestCase):

    expected_attributes = {
        'id' : 1,
        'channels' : 2
        }

    ##
    # \brief Tests the object initialisation of a ChannelGroup object
    def test_object_initialisation(self):
        obj = ChannelGroup(1, 2)
        self.assertTrue(isinstance(obj, ChannelGroup))
        check_expected_attributes(self, obj, self.expected_attributes)


##
# \brief Tests the Setting class
class Test_Setting(unittest.TestCase):

    expected_attributes = {
        'datatype' : 1,
        'id' : 2,
        'name' : 3,
        'paramstreampos' : 4,
        'templateid' : 5,
        'sodindex' : 6,
        'sodsubindex' : 7
        }

    ##
    # \brief Tests the object initialisation of a Setting object
    def test_object_initialisation(self):
        obj = Setting(1, 2, 3, 4, 5, 6, 7)
        self.assertTrue(isinstance(obj, Setting))
        check_expected_attributes(self, obj, self.expected_attributes)


##
# \brief Tests the Settingsgroup class
class Test_SettingsGroup(unittest.TestCase):

    expected_attributes = {
        'id' : 1,
        'settings' : 2,
        'copyfromchannel' : 3
        }

    expected_attributes2 = {
        'id' : 1,
        'settings' : 2,
        'copyfromchannel' : None
        }

    ##
    # \brief Tests the object initialisation of a SettingsGroup object
    def test_object_initialisation(self):
        obj = SettingsGroup(1, 2, 3)
        self.assertTrue(isinstance(obj, SettingsGroup))
        check_expected_attributes(self, obj, self.expected_attributes)

        obj = SettingsGroup(1, 2)
        self.assertTrue(isinstance(obj, SettingsGroup))
        check_expected_attributes(self, obj, self.expected_attributes2)


##
# \brief Test the Function class
class Test_Function(unittest.TestCase):
    expected_attributes = {
        'id' : 1,
        'subfunctions' : 2
        }

    expected_attributes2 = {
        'id' : 1,
        'subfunctions' : []
        }

    ##
    # \brief Tests the object initialsiation of a Function object
    def test_object_initialisation(self):
        obj = Function(1, 2)
        self.assertTrue(isinstance(obj, Function))
        check_expected_attributes(self, obj, self.expected_attributes)

        obj = Function(1)
        self.assertTrue(isinstance(obj, Function))
        check_expected_attributes(self, obj, self.expected_attributes2)


##
# \brief Tests the SettingDataTemplate class
class Test_SettingDataTemplate(unittest.TestCase):
    expected_attributes = {
        'id' : 1,
        'datatype' : 2
        }

    ##
    # \brief Tests the object initialisation of a SettingDataTemplate object
    def test_object_initialisation(self):
        obj = SettingDataTemplate(1, 2)
        self.assertTrue(isinstance(obj, SettingDataTemplate))
        check_expected_attributes(self, obj, self.expected_attributes)


##
# \brief Tests the Transport class
class Test_Transport(unittest.TestCase):
    expected_attributes = {
        'name' : 1,
        'direction' : 2,
        'octetsize' : 3
        }

    ##
    # \brief Tests the object initialisation of a Transport object
    def test_object_initialisation(self):
        obj = Transport(1, 2, 3)
        self.assertTrue(isinstance(obj, Transport))
        check_expected_attributes(self, obj, self.expected_attributes)


##
# \brief Tests the SPDOTransport class
class Test_SPDOTransport(unittest.TestCase):
    expected_attributes = {
        'name' : 1,
        'direction' : 2,
        'octetsize' : 3,
        'channels' : []
        }

    ##
    # \brief Tests the initialisation of a SPDOTransport object
    def test_object_initialisation(self):
        obj = SPDOTransport(1, 2, 3)
        self.assertTrue(isinstance(obj, SPDOTransport))
        check_expected_attributes(self, obj, self.expected_attributes)


##
# \brief Tests the OSDDSettingDataTemplateElements class
class Test_OSDDSettingDataTemplateElements(unittest.TestCase):
    expected_attributes = {
        'Choice' : 'Choice',
        'Range' : 'Range',
        'ParameterConstants' : 'ParameterConstants'
        }

    ##
    # \brief Tests the class for expected attributes and their values
    def test_OSDDSettingDataTemplateElements(self):
        check_expected_attributes(self, OSDDSettingDataTemplateElements,
                                  self.expected_attributes)


##
# \brief Tests the OSDDVisibility class
class Test_OSDDVisibility(unittest.TestCase):
    expected_attributes = {
        'Overall' : 'Overall',
        'ReadOnly' : 'ReadOnly',
        'SafetyOnly' : 'SafetyOnly',
        'None_' : 'None'
        }

    ##
    # \brief Tests the class for expected attributes and their values
    def test_OSDDVisibility(self):
        check_expected_attributes(self, OSDDVisibility,
                                  self.expected_attributes)


##
# \brief Tests the class OSDDDirections
class Test_OSDDDirections(unittest.TestCase):
    expected_attributes = {
        'Input' : 'Input',
        'Output' : 'Output'
        }

    ##
    # \brief Tests the class for expected attributes and their values
    def test_OSDDDirections(self):
        check_expected_attributes(self, OSDDDirections,
                                  self.expected_attributes)


##
# \brief Tests the class OSDDValueType
class Test_OSDDValueType(unittest.TestCase):
    expected_attributes = {
        'datatype' : 1,
        'size' : 2,
        'nextbiggertype' : 3
        }

    expected_attributes2 = {
        'datatype' : 1,
        'size' : 2,
        'nextbiggertype' : None
        }

    ##
    # \brief Tests the object initialisation of the class for expected
    # attributes and values
    def test_OSDDValueType(self):
        obj = OSDDValueType(1, 2, 3)
        check_expected_attributes(self, obj, self.expected_attributes)

        obj = OSDDValueType(1, 2)
        check_expected_attributes(self, obj, self.expected_attributes2)

    ##
    # \brief Tests the operator '=='
    def test_equality_operator(self):
        obj = OSDDValueType(1, 2, 3)
        self.assertTrue(obj == 1)

        obj = OSDDValueType(None, 2, 3)
        self.assertTrue(obj == None)

        obj = OSDDValueType('DataType', 2, 3)
        self.assertTrue(obj == 'DataType')

    ##
    # \brief Tests the __repr__ method for usage of the expected attribute
    def test_repr_method(self):
        obj = OSDDValueType(1, 2, 3)
        self.assertTrue(repr(obj) == repr(1))

        obj = OSDDValueType(None, 2, 3)
        self.assertTrue(repr(obj) == repr(None))

        obj = OSDDValueType('DataType', 2, 3)
        self.assertTrue(repr(obj) == repr('DataType'))

    ##
    # \brief Tests the operator '<'
    def test_less_than_operator(self):
        obj1 = OSDDValueType(1, 2, 3)
        obj2 = OSDDValueType(4, 5, 6)

        self.assertTrue(obj1 < obj2)
        self.assertFalse(obj2 < obj1)

        self.assertFalse(obj1 < None)
        self.assertFalse(obj2 < None)


##
# \brief Tests the class OSDDValueTypes
class Test_OSDDValueTypes(unittest.TestCase):

    expected_attributes = {
        'Boolean' : OSDDValueType('Boolean' , 1),
        'UInt8' : OSDDValueType('UInt8', 1,),
        'UInt16' : OSDDValueType('UInt16', 2),
        'UInt24' : OSDDValueType('UInt24', 3),
        'UInt32' : OSDDValueType('UInt32', 4),
        'UInt40' : OSDDValueType('UInt40', 5),
        'UInt48' : OSDDValueType('UInt48', 6),
        'UInt56' : OSDDValueType('UInt56', 7),
        'UInt64' : OSDDValueType('UInt64', 8),
        'Int8' : OSDDValueType('Int8', 1),
        'Int16' : OSDDValueType('Int16', 2),
        'Int24' : OSDDValueType('Int24', 3),
        'Int32' : OSDDValueType('Int32', 4),
        'Int40' : OSDDValueType('Int40', 5),
        'Int48' : OSDDValueType('Int48', 6),
        'Int56' : OSDDValueType('Int56', 7),
        'Int64' : OSDDValueType('Int64', 8),
        'Float32' : OSDDValueType('Float32', 4),
        'Float64' : OSDDValueType('Float64', 8)
    }

    ##
    # \brief Tests the class for expected attributes and values
    def test_OSDDValueTypes(self):
        for item in self.expected_attributes.items():
            key = item[0]
            value = item[1]
            self.assertTrue(hasattr(OSDDValueTypes, key))
            attr = getattr(OSDDValueTypes, key)
            self.assertEqual(attr.datatype, value.datatype)
            self.assertEqual(attr.size, value.size)

    ##
    # \brief Tests the method get_type() with valid and invalid types
    def test_OSDDValueTypes_get_type(self):
        for item in self.expected_attributes:
            atype = OSDDValueTypes.get_type(item)
            self.assertIsNotNone(atype)
            self.assertTrue(isinstance(atype, OSDDValueType))

        atype = OSDDValueTypes.get_type('UnknownDataType')
        self.assertIsNone(atype)

        atype = OSDDValueTypes.get_type(None)
        self.assertIsNone(atype)

    ##
    # \brief Tests the method get_size_of_type() with valid and invalid types
    def test_OSDDValueTypes_get_size_of_type(self):
        for item in self.expected_attributes.items():
            key = item[0]
            value = item[1]
            size = OSDDValueTypes.get_size_of_type(key)
            self.assertIsNotNone(size)
            self.assertEqual(size, value.size)

        size = OSDDValueTypes.get_size_of_type('UnknownDataType')
        self.assertIsNone(size)

        size = OSDDValueTypes.get_size_of_type(None)
        self.assertIsNone(size)


##
# \brief Tests the function get_sorted_settings_list()
class Test_get_sorted_settings_list(unittest.TestCase):

    ##
    # \brief Helper method for transforming OSDDSetting objects to
    # Setting objects, imitates a part of the tested functions behaviour
    def osddsetting_to_setting(self, osddsetting):

        paramstreampos = None
        sodindex = None
        subindex = None

        datatype = osddsetting.Type
        if osddsetting.DataSelection:
            datatype = osddsetting.DataSelection.Type

        if osddsetting.ParameterStream:
            paramstreampos = osddsetting.ParameterStream.Position

        if osddsetting.SODPosition:
            sodindex = osddsetting.SODPosition.Index
            subindex = osddsetting.SODPosition.SubIndex


        return Setting(datatype, osddsetting.id,
                       osddsetting.Name, paramstreampos, osddsetting.Template,
                       sodindex, subindex)

    ##
    # \brief Tests the function wit an empty list
    def test_empty_list(self):
        settingslist = []

        result = get_sorted_settings_list(settingslist)
        self.assertEqual(result, settingslist)

    ##
    # \brief Test the function with OSDDSetting objects
    def test_non_empty_list(self):

        s1 = DummyOSDDSetting('PS3Setting', 'Settingname', None, None,
                              'DataSelectionType', None, None, 'Overall',
                              None, None)
        s2 = DummyOSDDSetting('PS2Setting', 'Settingname', 'Template3', 'Int32',
                              None, None, None, 'Overall', 2, None)
        s3 = DummyOSDDSetting('PS1Setting', 'Settingname', None, 'Int32',
                              None, None, None, 'Overall', 1, None)

        s4 = DummyOSDDSetting('SPSetting1', 'Settingname', None, None,
                              'DataSelectionType', None, None, 'Overall',
                              None, (0x1234, 0x02))
        s5 = DummyOSDDSetting('SPSetting2', 'Settingname', None, 'UInt8',
                              None, None, None, 'Overall', None, (0x1234, 0x01))

        s6 = DummyOSDDSetting('SPSetting3', 'Settingname', None, 'UInt8',
                              None, None, None, 'SafetyOnly', None,
                              (0x2345, 0x0))
        s7 = DummyOSDDSetting('SPSetting4', 'Settingname', None, 'UInt8',
                              None, None, None, 'SafetyOnly', None,
                              (0x2345, 0x0))

        s8 = DummyOSDDSetting('NotTransmittedSetting', 'Settingname', None,
                              'UInt32', None, None, None, 'None', None,
                              (0x3456, 0x00))
        s9 = DummyOSDDSetting('NotTransmittedSetting2', 'Settingname', None,
                              'UInt32', None, None, None, 'None', 1, None)

        settingslist = [s1, s2, s3, s4, s5, s6, s7, s8, s9]
        expected_result_OSDD = [s3, s2, s1, s4, s5, s6, s7]
        expected_result = []

        # transform OSDDSetting objects to Setting objects to verify
        # them as expected result, because the tested function will
        # also transfrom OSDDSetting objects into Setting objects
        for item in expected_result_OSDD:
            expected_result.append(self.osddsetting_to_setting(item))

        result = get_sorted_settings_list(settingslist)
        self.assertEqual(len(result), len(expected_result))

        for i in range (len(expected_result)):
            for attribute in [attr for attr in dir(expected_result[i]) if not
                              callable(attr) and not attr.startswith('__')]:
                self.assertEqual(
                    getattr(result[i], attribute),
                    getattr(expected_result[i], attribute))

if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
