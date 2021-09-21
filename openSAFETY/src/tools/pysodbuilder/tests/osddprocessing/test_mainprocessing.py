##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_mainprocessing.py
# Unit test for the module pysodb.osddprocessing.mainprocessing

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
from tests.common import MagicDummy

from pysodb.util.sod_data import SODEntryNumEntries, SODDefaultValue, SODEntry
from pysodb.util.macro import BitAccessMacro


##
# \brief checks a list with SODEntry objects for expected entries: length, index
# subindex
def check_expected_entries(testcase, sodentrylist, expected_number_of_entries,
                           startindex, start_subindex):
    testcase.assertEqual(len(sodentrylist), expected_number_of_entries)

    for i in range(len(sodentrylist)):
        testcase.assertEqual(sodentrylist[i].objectindex, startindex)
        testcase.assertEqual(sodentrylist[i].subindex, start_subindex + i)


##
# \brief Tests MainProcessing class and its methods
class Test_MainProcessing(unittest.TestCase):

    ##
    # \brief Tests the do_mainprocessing() method for expected function calls
    def test_do_mainprocessing(self):
        from pysodb.osddprocessing.mainprocessing import MainProcessing

        #save original function references
        orig_generate_spdostructs_and_according_sod_entries = \
            MainProcessing._generate_spdostructs_and_according_sod_entries

        orig_make_error_register_sod_entry = \
            MainProcessing._make_error_register_sod_entry
        orig_make_error_statistics_sod_entries = \
            MainProcessing._make_error_statistics_sod_entries

        orig_make_life_guarding_sod_entries = \
            MainProcessing._make_life_guarding_sod_entries
        orig_make_number_of_retrties_reset_guarding_sod_entry = \
            MainProcessing._make_number_of_retrties_reset_guarding_sod_entry
        orig_make_refresh_interval_of_reset_guarding_sod_entry = \
            MainProcessing._make_refresh_interval_of_reset_guarding_sod_entry
        orig_make_device_vendor_information_sod_entries = \
            MainProcessing._make_device_vendor_information_sod_entries
        orig_make_unique_device_id_sod_entry = \
            MainProcessing._make_unique_device_id_sod_entry
        orig_make_parameter_download_sod_entry = \
            MainProcessing._make_parameter_download_sod_entry
        orig_make_common_communication_parameter_sod_entries = \
            MainProcessing._make_common_communication_parameter_sod_entries

        orig_make_mapping_sod_entries = MainProcessing._make_mapping_sod_entries
        orig_make_tx_com_sod_entries = MainProcessing._make_tx_com_sod_entries
        orig_make_rx_com_sod_entries = MainProcessing._make_rx_com_sod_entries

        orig_generate_settings_structs_paramstream = \
            MainProcessing._generate_settings_structs_paramstream
        orig_generate_settings_sodpos_struct_and_according_sod_entries = \
            MainProcessing._generate_settings_sodpos_struct_and_according_sod_entries
        orig_make_vendor_module_specific_sod_entries = \
            MainProcessing._make_vendor_module_specific_sod_entries
        orig_make_end_sod_entry = MainProcessing._make_end_sod_entry

        #patch with dummy
        MainProcessing._generate_spdostructs_and_according_sod_entries = \
            MagicDummy()

        MainProcessing._make_error_register_sod_entry = MagicDummy()
        MainProcessing._make_error_statistics_sod_entries = MagicDummy()

        MainProcessing._make_life_guarding_sod_entries = MagicDummy()
        MainProcessing._make_number_of_retrties_reset_guarding_sod_entry = \
            MagicDummy()
        MainProcessing._make_refresh_interval_of_reset_guarding_sod_entry = \
            MagicDummy()
        MainProcessing._make_device_vendor_information_sod_entries = \
            MagicDummy()
        MainProcessing._make_unique_device_id_sod_entry = MagicDummy()
        MainProcessing._make_parameter_download_sod_entry = MagicDummy()
        MainProcessing._make_common_communication_parameter_sod_entries = \
            MagicDummy()

        MainProcessing._make_mapping_sod_entries = MagicDummy()
        MainProcessing._make_tx_com_sod_entries = MagicDummy()
        MainProcessing._make_rx_com_sod_entries = MagicDummy()

        MainProcessing._generate_settings_structs_paramstream = MagicDummy()
        MainProcessing._generate_settings_sodpos_struct_and_according_sod_entries = \
            MagicDummy()
        MainProcessing._make_vendor_module_specific_sod_entries = MagicDummy()
        MainProcessing._make_end_sod_entry = MagicDummy()

        #check values
        processingdata = MagicDummy()
        obj = MainProcessing(processingdata)
        obj.do_mainprocessing()

        self.assertEqual(
            obj._generate_spdostructs_and_according_sod_entries.get_function_call_params(),
            [((), {})])

        self.assertEqual(
            obj._make_error_register_sod_entry.get_function_call_params(),
            [((), {})])
        self.assertEqual(
            obj._make_error_statistics_sod_entries.get_function_call_params(),
            [((), {})])
        self.assertEqual(
            obj._make_life_guarding_sod_entries.get_function_call_params(),
            [((), {})])
        self.assertEqual(
            obj._make_number_of_retrties_reset_guarding_sod_entry.get_function_call_params(),
            [((), {})])
        self.assertEqual(
            obj._make_refresh_interval_of_reset_guarding_sod_entry.get_function_call_params(),
            [((), {})])
        self.assertEqual(
            obj._make_device_vendor_information_sod_entries.get_function_call_params(),
            [((), {})])
        self.assertEqual(
            obj._make_unique_device_id_sod_entry.get_function_call_params(),
            [((), {})])
        self.assertEqual(
            obj._make_parameter_download_sod_entry.get_function_call_params(),
            [((), {})])
        self.assertEqual(
            obj._make_common_communication_parameter_sod_entries.get_function_call_params(),
            [((), {})])

        self.assertEqual(
            obj._make_mapping_sod_entries.get_function_call_params(),
            [((), {})])
        self.assertEqual(
            obj._make_tx_com_sod_entries.get_function_call_params(),
            [((), {})])
        self.assertEqual(
            obj._make_rx_com_sod_entries.get_function_call_params(),
            [((), {})])
        self.assertEqual(
            obj._generate_settings_structs_paramstream.get_function_call_params(),
            [((), {})])
        self.assertEqual(
            obj._generate_settings_sodpos_struct_and_according_sod_entries.get_function_call_params(),
            [((), {})])

        self.assertEqual(
            obj._make_vendor_module_specific_sod_entries.get_function_call_params(),
            [((), {})])
        self.assertEqual(
            obj._make_end_sod_entry.get_function_call_params(),
            [((), {})])

        #restore original function references
        MainProcessing._generate_spdostructs_and_according_sod_entries = \
            orig_generate_spdostructs_and_according_sod_entries

        MainProcessing._make_error_register_sod_entry = \
            orig_make_error_register_sod_entry
        MainProcessing._make_error_statistics_sod_entries = \
            orig_make_error_statistics_sod_entries

        MainProcessing._make_life_guarding_sod_entries = \
            orig_make_life_guarding_sod_entries
        MainProcessing._make_number_of_retrties_reset_guarding_sod_entry = \
            orig_make_number_of_retrties_reset_guarding_sod_entry
        MainProcessing._make_refresh_interval_of_reset_guarding_sod_entry = \
            orig_make_refresh_interval_of_reset_guarding_sod_entry
        MainProcessing._make_device_vendor_information_sod_entries = \
            orig_make_device_vendor_information_sod_entries
        MainProcessing._make_unique_device_id_sod_entry = \
            orig_make_unique_device_id_sod_entry
        MainProcessing._make_parameter_download_sod_entry = \
            orig_make_parameter_download_sod_entry
        MainProcessing._make_common_communication_parameter_sod_entries = \
            orig_make_common_communication_parameter_sod_entries

        MainProcessing._make_mapping_sod_entries = orig_make_mapping_sod_entries
        MainProcessing._make_tx_com_sod_entries = orig_make_tx_com_sod_entries
        MainProcessing._make_rx_com_sod_entries = orig_make_rx_com_sod_entries

        MainProcessing._generate_settings_structs_paramstream = \
            orig_generate_settings_structs_paramstream
        MainProcessing._generate_settings_sodpos_struct_and_according_sod_entries = \
            orig_generate_settings_sodpos_struct_and_according_sod_entries
        MainProcessing._make_vendor_module_specific_sod_entries = \
            orig_make_vendor_module_specific_sod_entries
        MainProcessing._make_end_sod_entry = orig_make_end_sod_entry

    ##
    # \brief Tests the generation of SPDOStruct and SODEntry objects
    def test_generate_spdostructs_and_according_sod_entries(self):

        in_transport = MagicDummy()
        in_transport.direction = 'Input'
        in_transport.name = 'SafeIN'

        channel1 = MagicDummy()
        channel1.id = 'channel1id'
        channel1.sodindex = '0x6000'
        channel1.sodsubindex = '0x01'
        channel1.datatype = 'UInt8'
        channel2 = MagicDummy()
        channel2.id = 'channel2id'
        channel2.sodindex = '0x6000'
        channel2.sodsubindex = '0x02'
        channel2.datatype = 'Int32'
        in_transport.channels = [channel1, channel2]

        out_transport = MagicDummy()
        out_transport.direction = 'Output'
        out_transport.name = 'SafeOUT'
        channel3 = MagicDummy()
        channel3.id = 'channel3id'
        channel3.sodindex = '0x6200'
        channel3.sodsubindex = '0x01'
        channel3.datatype = 'Boolean'
        channel3.transportbitmask = '0x01'
        channel4 = MagicDummy()
        channel4.id = 'channel4id'
        channel4.sodindex = '0x6200'
        channel4.sodsubindex = '0x01'
        channel4.datatype = 'Boolean'
        channel4.transportbitmask = '0x02'
        out_transport.channels = [channel3, channel4]

        out_transport2 = MagicDummy()
        out_transport2.direction = 'Output'
        out_transport2.name = 'SafeOUT2'
        channel5 = MagicDummy()
        channel5.id = 'channel5id'
        channel5.sodindex = '0x6201'
        channel5.sodsubindex = '0x01'
        channel5.datatype = 'UInt16'
        out_transport2.channels = [channel5]

        processingdata = MagicDummy()

        processingdata.spdotransport_input = []
        processingdata.spdotransport_output = []

        processingdata.sodentries = []
        processingdata.spdo_data_structs = []
        processingdata.bit_access_macros = []

        from pysodb.osddprocessing.mainprocessing import MainProcessing

        obj = MainProcessing(processingdata)
        obj._generate_spdostructs_and_according_sod_entries()

        self.assertEqual(processingdata.sodentries, [])
        self.assertEqual(processingdata.spdo_data_structs, [])
        self.assertEqual(processingdata.bit_access_macros, [])

        processingdata.sodentries = []
        processingdata.spdo_data_structs = []
        processingdata.bit_access_macros = []
        processingdata.spdotransport_input = [in_transport]
        processingdata.spdotransport_output = [out_transport, out_transport2]

        obj._generate_spdostructs_and_according_sod_entries()

        #check macros
        expected_bit_access_macros = [
            ('channel3id', 'transpSafeOUT_g.SafeObjectData00', 0x01, 0),
            ('channel4id', 'transpSafeOUT_g.SafeObjectData00', 0x02, 1)]

        expected_sodentries = [
            (0x6000, 0x01, 'transpSafeIN_g.channel1id', 'U8'),
            (0x6000, 0x02, 'transpSafeIN_g.channel2id', 'I32'),
            (0x6000, 0x00, None, 'U8'),

            (0x6200, 0x01, 'transpSafeOUT_g.SafeObjectData00', 'U8'),
            (0x6200, 0x00, None, 'U8'),

            (0x6201, 0x01, 'transpSafeOUT2_g.channel5id', 'U16'),
            (0x6201, 0x00, None , 'U8'),
                        ]

        expected_structs_members = [
            ('tSPDOTransportSafeIN', 'transpSafeIN_g', [
                ('UINT8', 'channel1id'), ('INT32', 'channel2id')]),
            ('tSPDOTransportSafeOUT', 'transpSafeOUT_g', [
                ('UINT8', 'SafeObjectData00')]),
            ('tSPDOTransportSafeOUT2', 'transpSafeOUT2_g', [
                ('UINT16', 'channel5id')])
            ]

        self.assertEqual(len(processingdata.bit_access_macros),
                         len(expected_bit_access_macros))

        for i in range(len(processingdata.bit_access_macros)):
            macro = processingdata.bit_access_macros[i]
            self.assertIsInstance(macro, BitAccessMacro)
            self.assertEqual(macro.macroname, expected_bit_access_macros[i][0])
            self.assertEqual(macro.variablename,
                             expected_bit_access_macros[i][1])
            self.assertEqual(macro.bitmask, expected_bit_access_macros[i][2])
            self.assertEqual(macro.index, expected_bit_access_macros[i][3])

        #check sodentries
        self.assertEqual(len(processingdata.sodentries),
                         len(expected_sodentries))

        for i in range(len(expected_sodentries)):
            self.assertEqual(processingdata.sodentries[i].objectindex,
                             expected_sodentries[i][0])
            self.assertEqual(processingdata.sodentries[i].subindex,
                             expected_sodentries[i][1])
            self.assertEqual(processingdata.sodentries[i].objectdataref,
                             expected_sodentries[i][2])
            self.assertEqual(processingdata.sodentries[i].attributes.datatype,
                             expected_sodentries[i][3])

        #check struct and struct members
        self.assertEqual(len(processingdata.spdo_data_structs),
                         len(expected_structs_members))

        for i in range(len(expected_structs_members)):
            self.assertEqual(processingdata.spdo_data_structs[i].datatype,
                             expected_structs_members[i][0])
            self.assertEqual(processingdata.spdo_data_structs[i].identifier,
                             expected_structs_members[i][1])

            self.assertEqual(len(processingdata.spdo_data_structs[i].members),
                             len(expected_structs_members[i][2]))
            for k in range(len(expected_structs_members[i][2])):
                self.assertEqual(processingdata.spdo_data_structs[i].members[k].datatype,
                                 expected_structs_members[i][2][k][0])
                self.assertEqual(processingdata.spdo_data_structs[i].members[k].identifier,
                                 expected_structs_members[i][2][k][1])

    ##
    # \brief Tests the mehtod _make_error_register_sod_entry() for
    # expected SODEntry object
    def test_make_error_register_sod_entry(self):

        from pysodb.osddprocessing.mainprocessing import MainProcessing

        processingdata = MagicDummy()
        processingdata.sodentries = []

        obj = MainProcessing(processingdata)
        obj._make_error_register_sod_entry()

        check_expected_entries(self, processingdata.sodentries, 1, 0x1001, 0x00)

    ##
    # \brief Tests the mehtod _make_error_statistics_sod_entries() for
    # expected SODEntry objects
    def test_make_error_statistics_sod_entries(self):

        from pysodb.osddprocessing.mainprocessing import MainProcessing

        processingdata = MagicDummy()
        processingdata.sodentries = []
        processingdata.sodb_settings.errorStatistic = 'OFF'

        obj = MainProcessing(processingdata)
        obj._make_error_statistics_sod_entries()

        self.assertEqual(processingdata.sodentries, [])

        processingdata.sodentries = []
        processingdata.sodb_settings.errorStatistic = 'ON'

        obj = MainProcessing(processingdata)
        obj._make_error_statistics_sod_entries()

        check_expected_entries(self, processingdata.sodentries,
                               14, 0x1004, 0x00)

    ##
    # \brief Tests the mehtod _make_life_guarding_sod_entries() for
    # expected SODEntry objects
    def test_make_life_guarding_sod_entries(self):

        from pysodb.osddprocessing.mainprocessing import MainProcessing

        processingdata = MagicDummy()
        processingdata.sodentries = []

        obj = MainProcessing(processingdata)
        obj._make_life_guarding_sod_entries()

        check_expected_entries(self, processingdata.sodentries, 3, 0x100C, 0x00)

    ##
    # \brief Tests the mehtod
    # _make_number_of_retrties_reset_guarding_sod_entry() for expected
    # SODEntry object
    def test_make_number_of_retrties_reset_guarding_sod_entry(self):

        from pysodb.osddprocessing.mainprocessing import MainProcessing

        processingdata = MagicDummy()
        processingdata.sodentries = []

        obj = MainProcessing(processingdata)
        obj._make_number_of_retrties_reset_guarding_sod_entry()

        check_expected_entries(self, processingdata.sodentries, 1, 0x100D, 0x00)

    ##
    # \brief Tests the mehtod
    # _make_refresh_interval_of_reset_guarding_sod_entry() for expected SODEntry
    #  object
    def test_make_refresh_interval_of_reset_guarding_sod_entry(self):

        from pysodb.osddprocessing.mainprocessing import MainProcessing

        processingdata = MagicDummy()
        processingdata.sodentries = []

        obj = MainProcessing(processingdata)
        obj._make_refresh_interval_of_reset_guarding_sod_entry()

        check_expected_entries(self, processingdata.sodentries, 1, 0x100E, 0x00)

    ##
    # \brief Tests the mehtod _make_device_vendor_information_sod_entries() for
    # expected SODEntry objects
    def test_make_device_vendor_information_sod_entries(self):

        from pysodb.osddprocessing.mainprocessing import MainProcessing

        processingdata = MagicDummy()
        processingdata.sodentries = []

        obj = MainProcessing(processingdata)
        obj._make_device_vendor_information_sod_entries()

        check_expected_entries(self, processingdata.sodentries, 8, 0x1018, 0x00)

    ##
    # \brief Tests the mehtod _make_unique_device_id_sod_entry() for
    # expected SODEntry object
    def test_make_unique_device_id_sod_entry(self):

        from pysodb.osddprocessing.mainprocessing import MainProcessing

        processingdata = MagicDummy()
        processingdata.sodentries = []

        obj = MainProcessing(processingdata)
        obj._make_unique_device_id_sod_entry()

        check_expected_entries(self, processingdata.sodentries, 1, 0x1019, 0x00)

    ##
    # \brief Tests the mehtod _make_parameter_download_sod_entry() for
    # expected SODEntry object
    def test_make_parameter_download_sod_entry(self):

        from pysodb.osddprocessing.mainprocessing import MainProcessing

        processingdata = MagicDummy()
        processingdata.sodentries = []

        obj = MainProcessing(processingdata)
        obj._make_parameter_download_sod_entry()

        check_expected_entries(self, processingdata.sodentries, 1, 0x101A, 0x00)

    ##
    # \brief Tests the mehtod _make_common_communication_parameter_sod_entries()
    #  for expected SODEntry objects
    def test_make_common_communication_parameter_sod_entries(self):

        from pysodb.osddprocessing.mainprocessing import MainProcessing

        processingdata = MagicDummy()
        processingdata.sodentries = []

        obj = MainProcessing(processingdata)
        obj._make_common_communication_parameter_sod_entries()

        check_expected_entries(self, processingdata.sodentries, 5, 0x1200, 0x00)

    ##
    # \brief Tests the mehtod _make_mapping_sod_entries() for the expected
    # generation of SODEntry mapping objects
    def test_make_mapping_sod_entries(self):
        from pysodb.osddprocessing.mainprocessing import MainProcessing

        orig_make_mapping_sod_entries_for_transportlist = \
            MainProcessing._make_mapping_sod_entries_for_transportlist
        dummyfunc = MagicDummy()

        processingdata = MagicDummy()
        processingdata.spdotransport_output = 'spdotransport_output'
        processingdata.spdotransport_input = 'spdotransport_input'

        expected_attrlist = ['RW', 'CRC', 'BEF_WR']

        obj = MainProcessing(processingdata)
        obj._make_mapping_sod_entries_for_transportlist = dummyfunc
        obj._make_mapping_sod_entries()

        self.assertEqual(
            obj._make_mapping_sod_entries_for_transportlist.get_function_call_params(),
            [(('spdotransport_output', 0x1800, expected_attrlist), {}),
             (('spdotransport_input', 0xC000, expected_attrlist), {})])

        MainProcessing._make_mapping_sod_entries_for_transportlist = \
            orig_make_mapping_sod_entries_for_transportlist

    ##
    # \brief Tests the mehtod _make_tx_com_sod_entries() for the expected
    # TX communication parameter SODEntry objects
    def test_make_tx_com_sod_entries(self):

        from pysodb.osddprocessing.mainprocessing import MainProcessing
        processingdata = MagicDummy()

        processingdata.spdotransport_input = []
        processingdata.sodentries = []
        obj = MainProcessing(processingdata)
        obj._make_tx_com_sod_entries()

        transport1 = MagicDummy()
        transport2 = MagicDummy()

        processingdata.spdotransport_input = [transport1, transport2]

        obj._make_tx_com_sod_entries()

        expected_values = [
                           (0x1C00, 0x00),
                           (0x1C00, 0x01),
                           (0x1C00, 0x02),
                           (0x1C00, 0x03),
                           (0x1C01, 0x00),
                           (0x1C01, 0x01),
                           (0x1C01, 0x02),
                           (0x1C01, 0x03)
                           ]
        self.assertEqual(len(processingdata.sodentries), len(expected_values))

        for i in range(len(expected_values)):
            self.assertEqual(processingdata.sodentries[i].objectindex,
                             expected_values[i][0])
            self.assertEqual(processingdata.sodentries[i].subindex,
                             expected_values[i][1])

    ##
    # \brief Tests the mehtod _make_tx_com_sod_entries() for the expected
    # RX communication parameter SODEntry objects
    def test_make_rx_com_sod_entries(self):
        from pysodb.osddprocessing.mainprocessing import MainProcessing
        processingdata = MagicDummy()

        processingdata.spdotransport_output = []
        processingdata.sodentries = []
        obj = MainProcessing(processingdata)
        obj._make_rx_com_sod_entries()

        transport1 = MagicDummy()
        transport2 = MagicDummy()

        processingdata.spdotransport_output = [transport1, transport2]

        obj._make_rx_com_sod_entries()

        expected_values = [
                           (0x1400, 0x00),
                           (0x1400, 0x01),
                           (0x1400, 0x02),
                           (0x1400, 0x03),
                           (0x1400, 0x04),
                           (0x1400, 0x05),
                           (0x1400, 0x06),
                           (0x1400, 0x07),
                           (0x1400, 0x08),
                           (0x1400, 0x09),
                           (0x1400, 0x0A),
                           (0x1400, 0x0B),
                           (0x1400, 0x0C),

                           (0x1401, 0x00),
                           (0x1401, 0x01),
                           (0x1401, 0x02),
                           (0x1401, 0x03),
                           (0x1401, 0x04),
                           (0x1401, 0x05),
                           (0x1401, 0x06),
                           (0x1401, 0x07),
                           (0x1401, 0x08),
                           (0x1401, 0x09),
                           (0x1401, 0x0A),
                           (0x1401, 0x0B),
                           (0x1401, 0x0C)
                           ]
        self.assertEqual(len(processingdata.sodentries), len(expected_values))

        for i in range(len(expected_values)):
            self.assertEqual(processingdata.sodentries[i].objectindex,
                             expected_values[i][0])
            self.assertEqual(processingdata.sodentries[i].subindex,
                             expected_values[i][1])

    ##
    # \brief Tests the mehtod _generate_settings_structs_paramstream() for the
    # correct generation of Struct objects for parameter stream settings
    def test_generate_settings_structs_paramstream(self):
        from pysodb.osddprocessing.mainprocessing import MainProcessing

        processingdata = MagicDummy()

        settingsgroup1 = MagicDummy()
        settingsgroup1.id = 'settingsgroup1id'

        sg1setting1 = MagicDummy()
        sg1setting1.sodindex = '0x1234'

        sg1setting2 = MagicDummy()
        sg1setting2.sodindex = None
        sg1setting2.datatype = 'Boolean'
        sg1setting2.id = 'sg1setting2id'

        sg1setting3 = MagicDummy()
        sg1setting3.sodindex = None
        sg1setting3.datatype = 'UInt16'
        sg1setting3.id = '2_3'
        sg1setting3.name = 'sg1setting3name'

        settingsgroup1.settings = [sg1setting1, sg1setting2, sg1setting3]

        settingsgroup2 = MagicDummy()
        settingsgroup2.id = 'settingsgroup2id'
        sg2setting1 = MagicDummy()
        sg2setting1.sodindex = None
        sg2setting1.datatype = 'Int32'
        sg2setting1.id = '3_0'
        sg2setting1.name = '0invalidname'

        settingsgroup2.settings = [sg2setting1]

        settingsgroup3 = MagicDummy()
        settingsgroup3.settings = []

        processingdata.settingsgroups = []
        processingdata.settingsgroups_struct_paramstream = None
        processingdata.settings_structs_paramstream = []

        obj = MainProcessing(processingdata)

        obj._generate_settings_structs_paramstream()

        self.assertEqual(processingdata.settingsgroups_struct_paramstream, None)
        self.assertEqual(processingdata.settings_structs_paramstream, [])

        processingdata.settingsgroups = [settingsgroup1,
                                         settingsgroup2,
                                         settingsgroup3]

        obj._generate_settings_structs_paramstream()

        expected_structs_members = [('tsettingsgroup1idParamStreamPos',
                                     'settingsgroup1id',
                                     [('UINT8', 'sg1setting2id'),
                                      ('UINT16', 'sg1setting3name')]
                                     ),
                                    ('tsettingsgroup2idParamStreamPos',
                                     'settingsgroup2id',
                                     [('INT32', 'SettingsData0')]
                                     )
                                   ]

        self.assertIsNotNone(processingdata.settingsgroups_struct_paramstream)
        self.assertEqual(
            processingdata.settingsgroups_struct_paramstream.datatype,
            'tSettingsGroups')
        self.assertEqual(
            processingdata.settingsgroups_struct_paramstream.identifier,
            'manParams')

        self.assertEqual(len(processingdata.settings_structs_paramstream),
                         len(expected_structs_members))

        self.assertEqual(
            processingdata.settingsgroups_struct_paramstream.members,
            processingdata.settings_structs_paramstream)

        for i in range(len(expected_structs_members)):
            self.assertEqual(
                processingdata.settings_structs_paramstream[i].datatype,
                expected_structs_members[i][0])
            self.assertEqual(
                processingdata.settings_structs_paramstream[i].identifier,
                expected_structs_members[i][1])

            for k in range(len(expected_structs_members[i][2])):
                self.assertEqual(
                    processingdata.settings_structs_paramstream[i].members[k].datatype,
                     expected_structs_members[i][2][k][0])
                self.assertEqual(
                    processingdata.settings_structs_paramstream[i].members[k].identifier,
                    expected_structs_members[i][2][k][1])

    ##
    # \brief Tests the mehtod
    # _generate_settings_sodpos_struct_and_according_sod_entries() for the
    # correct generation of Struct objects for settings with SOD positions
    # and  their belonging SODEntry objects
    def test_generate_settings_sodpos_struct_and_according_sod_entries(self):
        from pysodb.osddprocessing.mainprocessing import MainProcessing

        processingdata = MagicDummy()
        settingsgroup1 = MagicDummy()
        settingsgroup1.id = 'settingsgroup1id'

        sg1setting1 = MagicDummy()
        sg1setting1.sodindex = None

        sg1setting2 = MagicDummy()
        sg1setting2.sodindex = '0x2345'
        sg1setting2.sodsubindex = '0x01'
        sg1setting2.datatype = 'Boolean'
        sg1setting2.id = 'sg1setting2id'

        sg1setting3 = MagicDummy()
        sg1setting3.sodindex = '0x1234'
        sg1setting3.sodsubindex = '0x02'
        sg1setting3.datatype = 'UInt16'
        sg1setting3.id = '2_3'
        sg1setting3.name = 'sg1setting3name'

        settingsgroup1.settings = [sg1setting1, sg1setting2, sg1setting3]

        settingsgroup2 = MagicDummy()
        settingsgroup2.id = 'settingsgroup2id'
        sg2setting1 = MagicDummy()
        sg2setting1.sodindex = '0x0123'
        sg2setting1.sodsubindex = '0x00'
        sg2setting1.datatype = 'Int32'
        sg2setting1.id = '3_0'
        sg2setting1.name = '0invalidname'

        settingsgroup2.settings = [sg2setting1]

        settingsgroup3 = MagicDummy()
        settingsgroup3.settings = []

        processingdata.settingsgroups = []
        processingdata.settings_sodpos_struct = None

        processingdata.sodentries = []

        obj = MainProcessing(processingdata)
        obj._generate_settings_sodpos_struct_and_according_sod_entries()

        self.assertEqual(processingdata.sodentries, [])
        self.assertIsNone(processingdata.settings_sodpos_struct)

        processingdata.settingsgroups = [settingsgroup1,
                                         settingsgroup2,
                                         settingsgroup3]
        obj._generate_settings_sodpos_struct_and_according_sod_entries()

        expected_sodentries = [
                                (0x2345, 0x01,
                                 'SettingsSODPos.sg1setting2id', 'U8'),
                                (0x1234, 0x02,
                                 'SettingsSODPos.sg1setting3name', 'U16'),
                                (0x0123, 0x00,
                                 'SettingsSODPos.SettingsData_0123_00', 'I32'),

                                (0x1234, 0x00, None , 'U8'),
                                (0x2345, 0x00, None , 'U8')
                                ]

        expected_struct_members = [('UINT8', 'sg1setting2id'),
                                   ('UINT16', 'sg1setting3name'),
                                   ('INT32', 'SettingsData_0123_00')]

        #check struct and struct members
        self.assertEqual(processingdata.settings_sodpos_struct.identifier,
                         'SettingsSODPos')
        self.assertEqual(processingdata.settings_sodpos_struct.datatype,
                         'tSettingsSODPos')
        self.assertEqual(len(processingdata.settings_sodpos_struct.members),
                         len(expected_struct_members))

        for i in range(len(expected_struct_members)):
            self.assertEqual(
                processingdata.settings_sodpos_struct.members[i].datatype,
                expected_struct_members[i][0])
            self.assertEqual(
                processingdata.settings_sodpos_struct.members[i].identifier,
                expected_struct_members[i][1])

        #check sod entries
        self.assertEqual(len(processingdata.sodentries),
                          len(expected_sodentries))

        for i in range(len(expected_sodentries)):
            self.assertEqual(
                processingdata.sodentries[i].objectindex,
                expected_sodentries[i][0])
            self.assertEqual(
                processingdata.sodentries[i].subindex,
                expected_sodentries[i][1])
            self.assertEqual(
                processingdata.sodentries[i].objectdataref,
                expected_sodentries[i][2])
            self.assertEqual(
                processingdata.sodentries[i].attributes.datatype,
                expected_sodentries[i][3])

    ##
    # \brief Tests the mehtod _make_vendor_module_specific_sod_entries() for
    # expected SODEntry objects
    def test_make_vendor_module_specific_sod_entries(self):
        processingdata = MagicDummy()
        processingdata.sodentries = []
        processingdata.settings_structs_paramstream = None

        from pysodb.osddprocessing.mainprocessing import MainProcessing

        expected_values = [(0x2001, 0x00)]

        obj = MainProcessing(processingdata)
        obj._make_vendor_module_specific_sod_entries()

        self.assertEqual(len(processingdata.sodentries), len(expected_values))

        for i in range(len(expected_values)):
            self.assertIsInstance(processingdata.sodentries[i], SODEntry)
            self.assertEqual(
                processingdata.sodentries[i].objectindex,
                expected_values[i][0])
            self.assertEqual(
                processingdata.sodentries[i].subindex,
                expected_values[i][1])


        processingdata.settings_structs_paramstream = True
        expected_values = [(0x2000, 0x00), (0x2001, 0x00)]
        processingdata.sodentries = []
        obj._make_vendor_module_specific_sod_entries()

        self.assertEqual(len(processingdata.sodentries), len(expected_values))

        for i in range(len(expected_values)):
            self.assertIsInstance(processingdata.sodentries[i], SODEntry)
            self.assertEqual(
                processingdata.sodentries[i].objectindex, expected_values[i][0])
            self.assertEqual(
                processingdata.sodentries[i].subindex, expected_values[i][1])

    ##
    # \brief Tests the mehtod _make_end_sod_entry() for expected SODEntryEnd
    # object
    def test_make_end_sod_entry(self):

        from pysodb.osddprocessing.mainprocessing import MainProcessing

        processingdata = MagicDummy()
        processingdata.sodentries = []

        obj = MainProcessing(processingdata)
        obj._make_end_sod_entry()

        self.assertEqual(len(processingdata.sodentries), 1)
        self.assertIsNone(processingdata.sodentries[0].objectindex)
        self.assertEqual(processingdata.sodentries[0].subindex, 255)

    ##
    # \brief Tests the mehtod _group_settingslist_by_sod_positions() for
    # correct grouping of settings by SOD positions
    def test_group_settingslist_by_sod_positions(self):

        setting0 = MagicDummy()
        setting0.sodindex = '0x3456'
        setting0.sodsubindex = '0x00'

        setting1 = MagicDummy()
        setting1.sodindex = '0x1234'
        setting1.sodsubindex = '0x01'

        setting2 = MagicDummy()
        setting2.sodindex = '0x1234'
        setting2.sodsubindex = '0x01'

        setting3 = MagicDummy()
        setting3.sodindex = '0x2345'
        setting3.sodsubindex = '0x00'

        setting4 = MagicDummy()
        setting4.sodindex = '0x3456'
        setting4.sodsubindex = '0x00'

        expected_values = {
           (0x3456, 0x00):[('0x3456', '0x00'), ('0x3456', '0x00')],
           (0x1234, 0x01):[('0x1234', '0x01'), ('0x1234', '0x01')],
           (0x2345, 0x00):[('0x2345', '0x00')]
           }

        settingslist = [setting0, setting1, setting2, setting3, setting4]

        from pysodb.osddprocessing.mainprocessing import MainProcessing

        processingdata = MagicDummy()

        obj = MainProcessing(processingdata)
        result_dict = obj._group_settingslist_by_sod_positions(settingslist)

        self.assertEqual(len(result_dict), len(expected_values))

        for item in expected_values.items():
            key = item[0]
            value = item[1]
            self.assertIn(key, result_dict)
            for i in range(len(value)):
                self.assertEqual(result_dict[key][i].sodindex, value[i][0])
                self.assertEqual(result_dict[key][i].sodsubindex, value[i][1])

    ##
    # \brief Tests the mehtod _group_channels_to_transports() for
    # correct grouping of channels by SOD positions
    def test_group_channels_to_transports(self):

        channel0 = MagicDummy()
        channel0.sodindex = '0x3456'
        channel0.sodsubindex = '0x00'

        channel1 = MagicDummy()
        channel1.sodindex = '0x1234'
        channel1.sodsubindex = '0x01'

        channel2 = MagicDummy()
        channel2.sodindex = '0x1234'
        channel2.sodsubindex = '0x01'

        channel3 = MagicDummy()
        channel3.sodindex = '0x2345'
        channel3.sodsubindex = '0x00'

        channel4 = MagicDummy()
        channel4.sodindex = '0x3456'
        channel4.sodsubindex = '0x00'

        expected_values = {
           (0x3456, 0x00):[('0x3456', '0x00'), ('0x3456', '0x00')],
           (0x1234, 0x01):[('0x1234', '0x01'), ('0x1234', '0x01')],
           (0x2345, 0x00):[('0x2345', '0x00')]
           }

        channels = [channel0, channel1, channel2, channel3, channel4]
        transport = MagicDummy()
        transport.channels = channels

        from pysodb.osddprocessing.mainprocessing import MainProcessing

        processingdata = MagicDummy()

        obj = MainProcessing(processingdata)
        result_dict = obj._group_channels_to_transports(transport)

        self.assertEqual(len(result_dict), len(expected_values))

        for item in expected_values.items():
            key = item[0]
            value = item[1]
            self.assertIn(key, result_dict)
            for i in range(len(value)):
                self.assertEqual(result_dict[key][i].sodindex, value[i][0])
                self.assertEqual(result_dict[key][i].sodsubindex, value[i][1])

    ##
    # \brief Tests the mehtod _get_biggest_datatype_of_channel_setting() for
    # correct determining the biggest data type in a list of channels
    def test_get_biggest_datatype_of_channel_setting(self):
        channel0 = MagicDummy()
        channel0.datatype = 'Boolean'

        channel1 = MagicDummy()
        channel1.datatype = 'UInt8'

        channel2 = MagicDummy()
        channel2.datatype = 'UInt64'

        channel3 = MagicDummy()
        channel3.datatype = 'UInt32'

        channel4 = MagicDummy()
        channel4.datatype = 'UInt16'

        channels = [channel0, channel1, channel2, channel3, channel4]

        from pysodb.osddprocessing.mainprocessing import MainProcessing

        processingdata = MagicDummy()

        obj = MainProcessing(processingdata)

        returnval = obj._get_biggest_datatype_of_channel_setting(channels)
        self.assertEqual(returnval, 'UInt64')

        channels = [channel0, channel4]

        returnval = obj._get_biggest_datatype_of_channel_setting(channels)
        self.assertEqual(returnval, 'UInt16')

        channels = [channel0]
        returnval = obj._get_biggest_datatype_of_channel_setting(channels)
        self.assertEqual(returnval, 'Boolean')

        channels = []
        returnval = obj._get_biggest_datatype_of_channel_setting(channels)
        self.assertEqual(returnval, None)

    ##
    # \brief Tests the mehtod _generate_bit_access_macros() for
    # correct generation of BitAccesss macro objects
    def test_generate_bit_access_macros(self):

        channel0 = MagicDummy()
        channel0.id = 'channel0id'
        channel0.transportbitmask = 0x04
        channel0.datatype = 'Boolean'

        channel1 = MagicDummy()
        channel1.datatype = 'UInt8'

        channel2 = MagicDummy()
        channel2.datatype = 'UInt64'

        channel3 = MagicDummy()
        channel3.id = 'channel3id'
        channel3.transportbitmask = '0x08'
        channel3.datatype = 'Boolean'


        channels = [channel0, channel1, channel2, channel3]
        from pysodb.osddprocessing.mainprocessing import MainProcessing

        processingdata = MagicDummy()
        processingdata.bit_access_macros = []

        obj = MainProcessing(processingdata)
        obj._generate_bit_access_macros(channels, 'StructInst', 'Identifier')

        expected_values = [('channel0id', 'StructInst.Identifier', 0x04, 2),
                           ('channel3id', 'StructInst.Identifier', 0x08, 3)]

        self.assertEqual(len(processingdata.bit_access_macros), 2)

        for i in range(len(processingdata.bit_access_macros)):
            macro = processingdata.bit_access_macros[i]
            self.assertIsInstance(macro, BitAccessMacro)
            self.assertEqual(macro.macroname, expected_values[i][0])
            self.assertEqual(macro.variablename, expected_values[i][1])
            self.assertEqual(macro.bitmask, expected_values[i][2])
            self.assertEqual(macro.index, expected_values[i][3])

    ##
    # \brief Tests the mehtod _get_identifier_datatype_from_channellist() for
    # generation of generic identifier names and correct determining of
    # the biggest data type used
    def test_get_identifier_datatype_from_channellist(self):

        channel1 = MagicDummy()
        channel1.id = 'channel1id'
        channel1.datatype = 'UInt8'

        channel2 = MagicDummy()
        channel2.datatype = 'Boolean'

        channel3 = MagicDummy()
        channel3.datatype = 'UInt32'

        countervar = 0
        channellist = [channel1]

        from pysodb.osddprocessing.mainprocessing import MainProcessing

        processingdata = MagicDummy()
        obj = MainProcessing(processingdata)

        for i in range(len(channellist)):
            identifier, datatype, countervar = \
                obj._get_identifier_datatype_from_channellist(channellist,
                                                              countervar)
            self.assertEqual(identifier, 'channel1id')
            self.assertEqual(countervar, i)
            self.assertEqual(datatype, 'UInt8')

        channellist = [channel1, channel2, channel3 ]
        countervar = 0

        for i in range(len(channellist)):
            identifier, datatype, countervar = \
                obj._get_identifier_datatype_from_channellist(channellist,
                                                              countervar)
            self.assertEqual(identifier, 'SafeObjectData' + '{:02d}'.format(i))
            self.assertEqual(countervar, i + 1)
            self.assertEqual(datatype, 'UInt32')

    ##
    # \brief Tests the mehtod _change_datatype_if_boolean()
    def test_change_datatype_if_boolean(self):
        from pysodb.osddprocessing.mainprocessing import MainProcessing

        processingdata = MagicDummy()

        obj = MainProcessing(processingdata)

        values_results = [('Boolean', 'UInt8'), ('UInt8', 'UInt8'),
                          ('NotABoolean', 'NotABoolean'), ('Int32', 'Int32')]

        for item in values_results:
            returnval = obj._change_datatype_if_boolean(item[0])
            self.assertEqual(returnval, item[1])

    ##
    # \brief Tests the mehtod _make_num_entries_sod_entries()
    # for generation of expected SODEntryNumEntries objects
    def test_make_num_entries_sod_entries(self):
        from pysodb.osddprocessing.mainprocessing import MainProcessing

        processingdata = MagicDummy()

        sod_pos_dict = {(0x1234, 0x01):None, (0x1234, 0x2): None,
                        (0x2345, 0x00) : None, (0x3456, 0x01): None}

        expected_entries = [(0x1234, 0x00), (0x3456, 0x00)]

        sodentries = []

        obj = MainProcessing(processingdata)
        obj._make_num_entries_sod_entries('aComment', sod_pos_dict, sodentries)

        self.assertEqual(len(sodentries), len(expected_entries))
        for i in range(len(expected_entries)):
            self.assertIsInstance(sodentries[i], SODEntryNumEntries)
            self.assertEqual(sodentries[i].objectindex, expected_entries[i][0])
            self.assertEqual(sodentries[i].subindex, 0x00)

    ##
    # \brief Tests the method
    # _make_structmember_translate_datatype_add_to_list() for correct
    # generation of Structmember objects and appending to a list
    def test_make_structmember_translate_datatype_add_to_list(self):
        from pysodb.osddprocessing.mainprocessing import MainProcessing

        processingdata = MagicDummy()
        struct = MagicDummy()
        struct.members = []

        obj = MainProcessing(processingdata)
        obj._make_structmember_translate_datatype_add_to_list(
            'aComment', struct, 'anIdentifier', 'NotanOSDDDataType')

        self.assertEqual(len(struct.members), 1)
        member = struct.members[0]
        self.assertEqual(member.datatype, None)
        self.assertEqual(member.comment, 'aComment')
        self.assertEqual(member.identifier, 'anIdentifier')

        obj._make_structmember_translate_datatype_add_to_list(
            'aComment2', struct, 'anIdentifier2', 'UInt32')

        self.assertEqual(len(struct.members), 2)
        member = struct.members[1]
        self.assertEqual(member.datatype, 'UINT32')
        self.assertEqual(member.comment, 'aComment2')
        self.assertEqual(member.identifier, 'anIdentifier2')

    ##
    # \brief Tests the mehtod _make_sodentry_translate_datatype_add_to_list()
    # for generation of expected SODEntry objects and appending to a list
    def test_make_sodentry_translate_datatype_add_to_list(self):
        from pysodb.osddprocessing.mainprocessing import MainProcessing

        processingdata = MagicDummy()

        sodentrylist = []

        struct = MagicDummy()
        struct.identifier = 'aStructIdentifier'

        obj = MainProcessing(processingdata)

        obj._make_sodentry_translate_datatype_add_to_list(
            struct, ['attributelist'], 'default_value', (0x1234, 0x56),
             'anIdentifier', 'NotanOSDDDataType', sodentrylist)

        self.assertEqual(len(sodentrylist), 1)
        self.assertEqual(sodentrylist[0].objectdataref,
                         'aStructIdentifier.anIdentifier')
        self.assertEqual(sodentrylist[0].objectindex, 0x1234)
        self.assertEqual(sodentrylist[0].subindex, 0x56)

        self.assertEqual(sodentrylist[0].attributes.attributes,
                         ['attributelist'])
        self.assertEqual(sodentrylist[0].attributes.defaultval_ref,
                         'default_value')
        self.assertEqual(sodentrylist[0].attributes.datatype, None)

    ##
    # \brief Tests the mehtod _make_mapping_sod_entries_for_transportlist()
    # for generation of expected mapping SODEntry objects
    def test_make_mapping_sod_entries_for_transportlist(self):

        from pysodb.osddprocessing.mainprocessing import MainProcessing

        in_transport = MagicDummy()
        in_transport.direction = 'Input'
        channel1 = MagicDummy()
        channel1.sodindex = '0x1234'
        channel1.sodsubindex = '0x01'
        channel2 = MagicDummy()
        channel2.sodindex = '0x1234'
        channel2.sodsubindex = '0x02'

        in_transport.channels = [channel1, channel2]

        in_transport2 = MagicDummy()
        in_transport2.direction = 'Input'
        channel3 = MagicDummy()
        channel3.sodindex = '0x5678'
        channel3.sodsubindex = '0x01'
        in_transport2.channels = [channel3]

        transportlist = [in_transport, in_transport2]
        objectindex = 0x1234
        attrlist = ['attr1', 'attr2']

        processingdata = MagicDummy()
        processingdata.sodentries = []

        obj = MainProcessing(processingdata)
        obj._make_mapping_sod_entries_for_transportlist(
            transportlist, objectindex, attrlist)

        expected_values = [
           (0x1234, 0x00,
            's_?INSTANCE?_act_general.aTxMapParam[0].ucNoMappingEntries'),
           (0x1234, 0x01,
            's_?INSTANCE?_act_general.aTxMapParam[0].aulMappingEntry[0]'),
           (0x1234, 0x02,
            's_?INSTANCE?_act_general.aTxMapParam[0].aulMappingEntry[1]'),
           (0x1235, 0x00,
            's_?INSTANCE?_act_general.aTxMapParam[1].ucNoMappingEntries'),
           (0x1235, 0x01,
            's_?INSTANCE?_act_general.aTxMapParam[1].aulMappingEntry[0]')
                           ]

        self.assertEqual(len(processingdata.sodentries), len(expected_values))

        for i in range(len(expected_values)):
            self.assertEqual(
                processingdata.sodentries[i].objectindex,
                expected_values[i][0])
            self.assertEqual(
                processingdata.sodentries[i].subindex,
                expected_values[i][1])
            self.assertEqual(
                processingdata.sodentries[i].objectdataref,
                expected_values[i][2])
            self.assertEqual(
                processingdata.sodentries[i].attributes.attributes,
                attrlist)

            self.assertIsNotNone(
                processingdata.sodentries[i].attributes.defaultval_ref)
            self.assertIsInstance(
                processingdata.sodentries[i].attributes.defaultval_ref,
                SODDefaultValue)

            if expected_values[i][1] == 0:
                self.assertEqual(
                    processingdata.sodentries[i].attributes.datatype, 'U8')
                self.assertEqual(
                    processingdata.sodentries[i].attributes.defaultval_ref.value,
                    '0x00')
            else:
                self.assertEqual(
                    processingdata.sodentries[i].attributes.datatype, 'U32')
                self.assertEqual(
                    processingdata.sodentries[i].attributes.defaultval_ref.value,
                    '0x00000000UL')

        out_transport = MagicDummy()
        out_transport.direction = 'Output'
        channel4 = MagicDummy()
        channel4.sodindex = '0x4567'
        channel4.sodsubindex = '0x01'
        out_transport.channels = [channel4]

        transportlist = [out_transport]
        objectindex = 0x2345
        attrlist = ['attr3']

        processingdata.sodentries = []

        obj = MainProcessing(processingdata)
        obj._make_mapping_sod_entries_for_transportlist(
            transportlist, objectindex, attrlist)

        expected_values = [
           (0x2345, 0x00,
            's_?INSTANCE?_act_general.aRxMapParam[0].ucNoMappingEntries'),
           (0x2345, 0x01,
            's_?INSTANCE?_act_general.aRxMapParam[0].aulMappingEntry[0]'),
                           ]

        self.assertEqual(len(processingdata.sodentries), len(expected_values))

        for i in range(len(expected_values)):
            self.assertEqual(
                processingdata.sodentries[i].objectindex,
                expected_values[i][0])
            self.assertEqual(
                processingdata.sodentries[i].subindex,
                expected_values[i][1])
            self.assertEqual(
                processingdata.sodentries[i].objectdataref,
                expected_values[i][2])
            self.assertEqual(
                processingdata.sodentries[i].attributes.attributes,
                attrlist)

            self.assertIsNotNone(
                processingdata.sodentries[i].attributes.defaultval_ref)
            self.assertIsInstance(
                processingdata.sodentries[i].attributes.defaultval_ref,
                SODDefaultValue)

            if expected_values[i][1] == 0:
                self.assertEqual(
                    processingdata.sodentries[i].attributes.datatype, 'U8')
                self.assertEqual(
                    processingdata.sodentries[i].attributes.defaultval_ref.value,
                    '0x00')
            else:
                self.assertEqual(
                    processingdata.sodentries[i].attributes.datatype, 'U32')
                self.assertEqual(
                    processingdata.sodentries[i].attributes.defaultval_ref.value,
                    '0x00000000UL')

    ##
    # \brief Tests the mehtod _get_generic_identifier_from_setting()
    # for generation of generic identifier names
    def test_get_generic_identifier_from_setting(self):
        setting0 = MagicDummy()
        setting0.id = 'setting0id'
        setting0.sodposition = 0x1234
        setting0.transportbitmask = 0x04
        setting0.datatype = 'Boolean'

        setting1 = MagicDummy()
        setting1.datatype = 'UInt16'

        setting2 = MagicDummy()
        setting2.datatype = 'UInt64'

        settingslist = [setting0]

        from pysodb.osddprocessing.mainprocessing import MainProcessing

        processingdata = MagicDummy()
        obj = MainProcessing(processingdata)
        retval = obj._get_generic_identifier_from_setting(
                                    settingslist, settingslist[0], '1234', '00')
        self.assertEqual(retval, ('setting0id', 'Boolean'))

        settingslist = [setting0, setting1]
        obj = MainProcessing(processingdata)
        retval = obj._get_generic_identifier_from_setting(
                                    settingslist, settingslist[0], '1234', '00')
        self.assertEqual(retval, ('SettingsData_1234_00', 'UInt16'))

        setting = MagicDummy()
        setting.datatype = 'Int16'
        setting.id = '2_3'
        setting.name = '2invalidname'
        settingslist = [setting]
        obj = MainProcessing(processingdata)
        retval = obj._get_generic_identifier_from_setting(
                                    settingslist, setting, '1234', '00')
        self.assertEqual(retval, ('SettingsData_1234_00', 'Int16'))

        setting.datatype = 'Int64'
        setting.name = 'ValidName'
        settingslist = [setting]
        obj = MainProcessing(processingdata)
        retval = obj._get_generic_identifier_from_setting(
                                    settingslist, setting, '1234', '00')
        self.assertEqual(retval, ('ValidName', 'Int64'))

if __name__ == '__main__':
    unittest.main()
##
# \}
# \}
# \}
