##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_common.py
# Unit test for the module pysodb.codegeneration.common

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
from tests.common import class_init_dummy, DummySODBsettings, DummyOSDDModule, \
    DummySPDOTransport, DummyDataStore

from pysodb.codegeneration.common import DataStore, FromSettingsDataStore, \
FromOSDDDataStore, getepls_enable_disable_from_string, DataChooser, Constants


expected_datastore_entries = [
        'epls_cfg_max_pyld_len',
        'epls_k_configuration_string',

        'spdo_cfg_max_no_tx_spdo',
        'spdo_cfg_max_tx_spdo_mapp_entries',
        'spdo_cfg_max_len_of_tx_spdo',

        'spdo_cfg_max_no_rx_spdo',
        'spdo_cfg_max_rx_spdo_mapp_entries',
        'spdo_cfg_max_len_of_rx_spdo',
        ]

expected_Constants_entries = {
    'EPLS_k_NOT_APPLICABLE' : 'EPLS_k_NOT_APPLICABLE',
    'EPLS_k_DISABLE' : 'EPLS_k_DISABLE',
    'EPLS_k_ENABLE' : 'EPLS_k_ENABLE',

    'SSDO_MIN_PAYLOAD_LENGTH' : 8,

    'FALSE_STRINGS' : ['False', 'OFF'],
    'TRUE_STRINGS' : ['True', 'ON'],
                              }


##
# \brief Tests the module pysodb.codegeneration.common
class Test_codegeneration_common(unittest.TestCase):

    def test_Constants(self):
        for item in expected_Constants_entries.items():
            key = item [0]
            value = item[1]

            self.assertTrue(hasattr(Constants, key))
            attr = getattr(Constants, key)
            self.assertEqual(attr, value)

    ##
    # \brief Tests getepls_enable_disable_from_string()
    def test_getepls_enable_disable_from_string(self):

        self.assertEqual(getepls_enable_disable_from_string('False'),
                         'EPLS_k_DISABLE')
        self.assertEqual(getepls_enable_disable_from_string('OFF'),
                         'EPLS_k_DISABLE')

        self.assertIsNone(getepls_enable_disable_from_string('false'))
        self.assertIsNone(getepls_enable_disable_from_string('off'))

        self.assertEqual(getepls_enable_disable_from_string('True'),
                         'EPLS_k_ENABLE')
        self.assertEqual(getepls_enable_disable_from_string('ON'),
                         'EPLS_k_ENABLE')

        self.assertIsNone(getepls_enable_disable_from_string('true'))
        self.assertIsNone(getepls_enable_disable_from_string('on'))

    ##
    # \brief Tests the DataStore base class
    def test_DataStore(self):
        ds = DataStore()
        for attribute in expected_datastore_entries:
            self.assertTrue(hasattr(ds, attribute))

    ##
    # \brief Tests the FromSettingsDataStore class with literal value
    def test_FromSettingsDataStore(self):
        sodbsettings = DummySODBsettings()

        fsds = FromSettingsDataStore(sodbsettings)

        self.assertEqual(fsds.epls_cfg_max_pyld_len, 0)
        self.assertEqual(fsds.epls_k_configuration_string, 'test')
        self.assertEqual(fsds.spdo_cfg_max_len_of_rx_spdo, 0)
        self.assertEqual(fsds.spdo_cfg_max_len_of_tx_spdo, 0)
        self.assertEqual(fsds.spdo_cfg_max_no_rx_spdo, 0)
        self.assertEqual(fsds.spdo_cfg_max_no_tx_spdo, 0)
        self.assertEqual(fsds.spdo_cfg_max_rx_spdo_mapp_entries, 0)
        self.assertEqual(fsds.spdo_cfg_max_tx_spdo_mapp_entries, 0)

        sodbsettings.constants.configString = 'newtest'
        sodbsettings.maxPayloadLength = 42
        sodbsettings.spdoCfg.rx.maxMapEntries = 100
        sodbsettings.spdoCfg.rx.maxPayloadSize = 101
        sodbsettings.spdoCfg.rx.maxSpdo = 102
        sodbsettings.spdoCfg.tx.maxMapEntries = 110
        sodbsettings.spdoCfg.tx.maxPayloadSize = 111
        sodbsettings.spdoCfg.tx.maxSpdo = 112

        fsds = FromSettingsDataStore(sodbsettings)

        self.assertEqual(fsds.epls_cfg_max_pyld_len, 42)
        self.assertEqual(fsds.epls_k_configuration_string, 'newtest')
        self.assertEqual(fsds.spdo_cfg_max_len_of_rx_spdo, 101)
        self.assertEqual(fsds.spdo_cfg_max_len_of_tx_spdo, 111)
        self.assertEqual(fsds.spdo_cfg_max_no_rx_spdo, 102)
        self.assertEqual(fsds.spdo_cfg_max_no_tx_spdo, 112)
        self.assertEqual(fsds.spdo_cfg_max_rx_spdo_mapp_entries, 100)
        self.assertEqual(fsds.spdo_cfg_max_tx_spdo_mapp_entries, 110)

        sodbsettings.constants.configString = None
        sodbsettings.maxPayloadLength = None
        sodbsettings.spdoCfg.rx.maxMapEntries = None
        sodbsettings.spdoCfg.rx.maxPayloadSize = None
        sodbsettings.spdoCfg.rx.maxSpdo = None
        sodbsettings.spdoCfg.tx.maxMapEntries = None
        sodbsettings.spdoCfg.tx.maxPayloadSize = None
        sodbsettings.spdoCfg.tx.maxSpdo = None

        with self.assertRaises((TypeError, ValueError)):
            fsds = FromSettingsDataStore(sodbsettings)


##
# \brief Tests FromOSDDDataStore class
class Test_FromOSDDDataStore(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)
        self.osddmodule = DummyOSDDModule()
        self.original_init = FromOSDDDataStore.__init__
        FromOSDDDataStore.__init__ = class_init_dummy

    def tearDown(self):
        unittest.TestCase.tearDown(self)
        FromOSDDDataStore.__init__ = self.original_init

    ##
    # \brief Tests the initialisation of the class
    def test_object_instantiation(self):

        self.osddmodule.ModuleDefinition.DataTransport.ACyclicTransport[0].\
            OctetSize = 24
        self.osddmodule.ModuleDefinition.DataTransport.ACyclicTransport[1].\
            OctetSize = 32

        self.osddmodule.ChannelConfiguration.DataTransport.SPDOTransport[0].\
            OctetSize = 15
        self.osddmodule.ChannelConfiguration.DataTransport.SPDOTransport[0].\
            Direction = 'Input'
        self.osddmodule.ChannelConfiguration.DataTransport.SPDOTransport[0].\
            MaxNrMappings = 4

        self.osddmodule.ChannelConfiguration.DataTransport.SPDOTransport[1].\
            OctetSize = 15
        self.osddmodule.ChannelConfiguration.DataTransport.SPDOTransport[1].\
            Direction = 'Output'
        self.osddmodule.ChannelConfiguration.DataTransport.SPDOTransport[1].\
            MaxNrMappings = 4
        self.osddmodule.id = 'moduleID'

        FromOSDDDataStore.__init__ = self.original_init
        fods = FromOSDDDataStore(self.osddmodule)

        for attribute in expected_datastore_entries:
            self.assertTrue(hasattr(fods, attribute))

    ##
    # \brief Tests get_max_acyclic_payload() for correct processing
    def test_get_max_acyclic_payload(self):

        fods = FromOSDDDataStore(self.osddmodule)
        fods._acyclic_transport = self.osddmodule.ModuleDefinition.\
            DataTransport.ACyclicTransport

        self.assertEqual(fods._get_max_acyclic_payload(), 0)

        self.osddmodule.ModuleDefinition.DataTransport.ACyclicTransport[0].\
            OctetSize = 24
        self.osddmodule.ModuleDefinition.DataTransport.ACyclicTransport[1].\
            OctetSize = 32

        fods._acyclic_transport = self.osddmodule.ModuleDefinition.\
            DataTransport.ACyclicTransport

        self.assertEqual(fods._get_max_acyclic_payload(), 4)

    ##
    # \brief Tests process_spdo_data() for correct processing
    def test_process_spdo_data(self):

        fods = FromOSDDDataStore(self.osddmodule)

        fods._max_octetsize_tx_spdo = 0
        fods._max_octetsize_rx_spdo = 0
        fods._maxnr_input_mappings = 0
        fods._maxnr_output_mappings = 0
        fods._spdotransports_input = []
        fods._spdotransports_output = []

        fods._spdotransports = self.osddmodule.ChannelConfiguration.\
            DataTransport.SPDOTransport

        fods._process_spdo_data()

        self.assertEqual(len(fods._spdotransports_input), 0)
        self.assertEqual(fods._maxnr_input_mappings, 0)
        self.assertEqual(fods._max_octetsize_tx_spdo, 0)

        self.assertEqual(len(fods._spdotransports_output), 0)
        self.assertEqual(fods._maxnr_output_mappings, 0)
        self.assertEqual(fods._max_octetsize_rx_spdo, 0)

        self.osddmodule.ModuleDefinition.DataTransport.ACyclicTransport[0].\
            OctetSize = 24
        self.osddmodule.ModuleDefinition.DataTransport.ACyclicTransport[1].\
            OctetSize = 32

        self.osddmodule.ChannelConfiguration.DataTransport.SPDOTransport[0].\
            OctetSize = 27
        self.osddmodule.ChannelConfiguration.DataTransport.SPDOTransport[0].\
            Direction = 'Input'
        self.osddmodule.ChannelConfiguration.DataTransport.SPDOTransport[0].\
            MaxNrMappings = 4

        self.osddmodule.ChannelConfiguration.DataTransport.SPDOTransport[1].\
            OctetSize = 31
        self.osddmodule.ChannelConfiguration.DataTransport.SPDOTransport[1].\
            Direction = 'Output'
        self.osddmodule.ChannelConfiguration.DataTransport.SPDOTransport[1].\
            MaxNrMappings = 5

        fods._process_spdo_data()

        self.assertEqual(len(fods._spdotransports_input), 1)
        self.assertEqual(fods._maxnr_input_mappings, 4)
        self.assertEqual(fods._max_octetsize_tx_spdo, 27)

        self.assertEqual(len(fods._spdotransports_output), 1)
        self.assertEqual(fods._maxnr_output_mappings, 5)
        self.assertEqual(fods._max_octetsize_rx_spdo, 31)


##
# \brief Tests the getter methods of FromOSDDDataStore
class Test_FromOSDDDataStore_getters(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)
        self.osddmodule = DummyOSDDModule()
        self.osddmodule.ModuleDefinition.DataTransport.ACyclicTransport[0].\
            OctetSize = 24
        self.osddmodule.ModuleDefinition.DataTransport.ACyclicTransport[1].\
            OctetSize = 32

        self.osddmodule.ChannelConfiguration.DataTransport.SPDOTransport[0].\
            OctetSize = 27
        self.osddmodule.ChannelConfiguration.DataTransport.SPDOTransport[0].\
            Direction = 'Input'
        self.osddmodule.ChannelConfiguration.DataTransport.SPDOTransport[0].\
            MaxNrMappings = 4

        self.osddmodule.ChannelConfiguration.DataTransport.SPDOTransport[1].\
            OctetSize = 31
        self.osddmodule.ChannelConfiguration.DataTransport.SPDOTransport[1].\
            Direction = 'Output'
        self.osddmodule.ChannelConfiguration.DataTransport.SPDOTransport[1].\
            MaxNrMappings = 5

    def tearDown(self):
        unittest.TestCase.tearDown(self)

    ##
    # \brief Test for expected returnvalues
    def test_get_maxnr_tx_mappings(self):

        fods = FromOSDDDataStore(self.osddmodule)
        self.assertIs(fods._get_maxnr_tx_mappings(), fods._maxnr_input_mappings)
        self.assertEqual(fods._get_maxnr_tx_mappings(), 4)

    ##
    # \brief Test for expected returnvalues
    def test_get_maxnr_rx_mappings(self):

        fods = FromOSDDDataStore(self.osddmodule)
        self.assertIs(fods._get_maxnr_rx_mappings(),
                      fods._maxnr_output_mappings)
        self.assertEqual(fods._get_maxnr_rx_mappings(), 5)

    ##
    # \brief Test for expected returnvalues
    def test_get_maxnr_tx_spdo(self):

        fods = FromOSDDDataStore(self.osddmodule)
        self.assertEqual(fods._get_maxnr_tx_spdo(), 1)

        spdotransport = DummySPDOTransport()
        spdotransport.Direction = 'Input'
        spdotransport.MaxNrMappings = 100
        spdotransport.OctetSize = 33

        self.osddmodule.ChannelConfiguration.DataTransport.SPDOTransport.\
            append(spdotransport)

        fods = FromOSDDDataStore(self.osddmodule)
        self.assertEqual(fods._get_maxnr_tx_spdo(), 2)

    ##
    # \brief Test for expected returnvalues
    def test_get_maxnr_rx_spdo(self):

        fods = FromOSDDDataStore(self.osddmodule)
        self.assertEqual(fods._get_maxnr_rx_spdo(), 1)

        spdotransport = DummySPDOTransport()
        spdotransport.Direction = 'Output'
        spdotransport.MaxNrMappings = 100
        spdotransport.OctetSize = 33

        self.osddmodule.ChannelConfiguration.DataTransport.SPDOTransport.\
            append(spdotransport)

        fods = FromOSDDDataStore(self.osddmodule)
        self.assertEqual(fods._get_maxnr_rx_spdo(), 2)

    ##
    # \brief Test for expected returnvalues
    def test_get_max_len_of_tx_spdo(self):

        fods = FromOSDDDataStore(self.osddmodule)
        self.assertEqual(fods._get_max_len_of_tx_spdo(), 8)

        spdotransport = DummySPDOTransport()
        spdotransport.Direction = 'Input'
        spdotransport.MaxNrMappings = 100
        spdotransport.OctetSize = 33

        self.osddmodule.ChannelConfiguration.DataTransport.SPDOTransport.\
            append(spdotransport)

        fods = FromOSDDDataStore(self.osddmodule)
        self.assertEqual(fods._get_max_len_of_tx_spdo(), 10)

    ##
    # \brief Test for expected returnvalues
    def test_get_max_len_of_rx_spdo(self):

        fods = FromOSDDDataStore(self.osddmodule)
        self.assertEqual(fods._get_max_len_of_rx_spdo(), 9)

        spdotransport = DummySPDOTransport()
        spdotransport.Direction = 'Output'
        spdotransport.MaxNrMappings = 100
        spdotransport.OctetSize = 19

        self.osddmodule.ChannelConfiguration.DataTransport.\
            SPDOTransport.append(spdotransport)

        fods = FromOSDDDataStore(self.osddmodule)
        self.assertEqual(fods._get_max_len_of_rx_spdo(), 9)

    ##
    # \brief Test for expected returnvalues, as well as expected behaviour
    def test_get_bytes_from_octetsize(self):

        fods = FromOSDDDataStore(self.osddmodule)

        testvalues_expected_results = [
            (10, ValueError), (11, 0), (12, ValueError), (13, 1),
            (19, 4), (20, ValueError), (21, 5), (26, ValueError),
            (27, 8), (28, ValueError), (29, ValueError), (30, ValueError),
            (31, 9), (32, ValueError), (33, 10)
            ]

        for item in testvalues_expected_results:
            if item[1] is ValueError:
                with self.assertRaises(ValueError):
                    fods._get_bytes_from_octetsize(item[0])
            else:
                self.assertEqual(fods._get_bytes_from_octetsize(item[0]),
                                 item[1])


##
# \brief Test for the DataChooser class
class Test_DataChooser(unittest.TestCase):

    ##
    # \brief Test of the object initialisation and instantiation
    def test_DataChooser_init(self):

        settings = DummyDataStore(expected_datastore_entries)
        osdd = DummyDataStore(expected_datastore_entries)

        dc = DataChooser(False, settings, osdd)
        self.assertEqual(dc.func, DataChooser._get_value_from_osdd)

        dc = DataChooser(True, settings, osdd)
        self.assertEqual(dc.func,
                         DataChooser._get_value_from_settings_if_bigger)

    ##
    # \brief Tests the getter method, attribute access and returnvalues
    def test_get(self):

        settings = DummyDataStore(expected_datastore_entries, 100)
        osdd = DummyDataStore(expected_datastore_entries, 0)

        dc = DataChooser(False, settings, osdd)

        expected_value = 0
        for item in expected_datastore_entries:
            self.assertIs(dc.get(item), getattr(osdd, item))
            self.assertEqual(dc.get(item), expected_value)
            expected_value += 1

        # test if data comes from fake FromSettingsDataStore (all values are
        # bigger than in osdd
        dc = DataChooser(True, settings, osdd)

        expected_value = 100
        for item in expected_datastore_entries:
            self.assertIs(dc.get(item), getattr(settings, item))
            self.assertEqual(dc.get(item), expected_value)
            expected_value += 1

        settings = DummyDataStore(expected_datastore_entries, 0)

        # only these two values should be taken from settings, the others from
        # osdd
        settings.epls_cfg_max_pyld_len = 100
        settings.spdo_cfg_max_no_tx_spdo = 200

        osdd = DummyDataStore(expected_datastore_entries, 0)

        dc = DataChooser(True, settings, osdd)

        for item in expected_datastore_entries:
            if item == 'epls_cfg_max_pyld_len' or \
            item == 'spdo_cfg_max_no_tx_spdo':
                self.assertIs(dc.get(item), getattr(settings, item))
                self.assertIsNot(dc.get(item), getattr(osdd, item))
            else:
                self.assertIs(dc.get(item), getattr(osdd, item))

    ##
    # \brief Test for expected returnvalues according to different values
    # between two DataStores
    def test_get_value_from_settings_if_bigger(self):

        settings = DummyDataStore(expected_datastore_entries)
        osdd = DummyDataStore(expected_datastore_entries)

        dc = DataChooser(False, settings, osdd)

        expected_value = 0
        for item in expected_datastore_entries:
            self.assertEqual(
                dc._get_value_from_settings_if_bigger(item, settings, osdd),
                                                    expected_value)
            self.assertIs(dc._get_value_from_settings_if_bigger(
                item, settings, osdd), getattr(osdd, item))

            expected_value += 1

        settings = DummyDataStore(expected_datastore_entries, 10)

        dc = DataChooser(False, settings, osdd)

        expected_value = 10
        for item in expected_datastore_entries:
            self.assertEqual(
                dc._get_value_from_settings_if_bigger(item, settings, osdd),
                                                    expected_value)
            self.assertIs(dc._get_value_from_settings_if_bigger(
                item, settings, osdd), getattr(settings, item))

            expected_value += 1

        del settings.epls_cfg_max_pyld_len

        dc = DataChooser(False, settings, osdd)

        with self.assertRaises(AttributeError):
            dc._get_value_from_settings_if_bigger('epls_cfg_max_pyld_len',
                                                  settings, osdd)

    ##
    # \brief Tests if every value is obtained from FromOSDDDataStore,
    # although both DataStores are present
    def test_get_value_from_osdd(self):

        settings = DummyDataStore(expected_datastore_entries)
        osdd = DummyDataStore(expected_datastore_entries)

        dc = DataChooser(False, settings, osdd)

        expected_value = 0
        for item in expected_datastore_entries:
            self.assertEqual(
                dc._get_value_from_osdd(item, settings, osdd),
                                                    expected_value)
            self.assertIs(dc._get_value_from_settings_if_bigger(
                item, settings, osdd), getattr(osdd, item))

            expected_value += 1

        settings = DummyDataStore(expected_datastore_entries, 10)

        dc = DataChooser(False, settings, osdd)

        expected_value = 0
        for item in expected_datastore_entries:
            self.assertEqual(
                dc._get_value_from_osdd(item, settings, osdd),
                                                    expected_value)
            self.assertIs(dc._get_value_from_osdd(
                item, settings, osdd), getattr(osdd, item))

            expected_value += 1

        del osdd.epls_cfg_max_pyld_len
        dc = DataChooser(False, settings, osdd)
        with self.assertRaises(AttributeError):
            dc._get_value_from_osdd('epls_cfg_max_pyld_len',
                                                  settings, osdd)

if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
