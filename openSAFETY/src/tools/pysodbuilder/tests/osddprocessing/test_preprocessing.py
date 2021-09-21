##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_preprocessing.py
# Unit test for the module pysodb.osddprocessing.preprocessing

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

from pysodb.util.osdd_data import SPDOTransport
import pysodb

##
# \brief Class for imitating SettingDataTemplate objects
class SimpleSDTDummy(object):
    def __init__(self, anid, atype):
        self.id = anid
        self.Type = atype


##
# \brief For patching out get_sorted_settings_list()
def dummy_get_sorted_settings_list(value):
    return value


##
# \brief Tests the PreProcessing class and its methods
class Test_PreProcessing(unittest.TestCase):

    ##
    # \brief Tests the do_processing method for expected function calls
    def test_do_processing(self):
        from pysodb.osddprocessing.preprocessing import PreProcessing
        orig_get_spdoinfo = PreProcessing._get_SPDOInfo
        orig_get_settingdatatemplates = PreProcessing._get_SettingDataTemplates
        orig_get_channels = PreProcessing._get_Channels
        orig_get_module_functions = PreProcessing._get_ModuleFunctions
        orig_get_modulesettings_settingsgroups = \
            PreProcessing._get_ModuleSettings_SettingsGroups
        orig__resolve_datatype = \
            PreProcessing._resolve_datatypes_from_template_references
        orig_link_channelsettings = \
            PreProcessing._link_channelsettings_into_module_settings
        orig_link_channels_into_spdo = \
            PreProcessing._link_channels_into_spdo_transports
        orig_get_used_channels = PreProcessing._get_used_channels

        PreProcessing._get_SPDOInfo = MagicDummy()
        PreProcessing._get_SettingDataTemplates = MagicDummy()
        PreProcessing._get_Channels = MagicDummy()
        PreProcessing._get_ModuleFunctions = MagicDummy()
        PreProcessing._get_ModuleSettings_SettingsGroups = MagicDummy()
        PreProcessing._resolve_datatypes_from_template_references = MagicDummy()
        PreProcessing._link_channelsettings_into_module_settings = MagicDummy()
        PreProcessing._link_channels_into_spdo_transports = MagicDummy()
        PreProcessing._get_used_channels = MagicDummy()

        processingdata = MagicDummy()

        obj = PreProcessing(processingdata)
        obj.do_preprocessing()

        self.assertEqual(
            obj._get_SPDOInfo.get_function_call_params(),
            [((), {})])
        self.assertEqual(
            obj._get_SettingDataTemplates.get_function_call_params(),
            [((), {})])
        self.assertEqual(
            obj._get_Channels.get_function_call_params(),
            [((), {})])
        self.assertEqual(
            obj._get_ModuleFunctions.get_function_call_params(),
            [((), {})])
        self.assertEqual(
            obj._get_ModuleSettings_SettingsGroups.get_function_call_params(),
            [((), {})])
        self.assertEqual(
            obj._resolve_datatypes_from_template_references.get_function_call_params(),
            [((), {})])
        self.assertEqual(
            obj._link_channelsettings_into_module_settings.get_function_call_params(),
            [((), {})])
        self.assertEqual(
            obj._link_channels_into_spdo_transports.get_function_call_params(),
            [((), {})])
        self.assertEqual(
            obj._get_used_channels.get_function_call_params(),
            [((), {})])

        PreProcessing._get_SPDOInfo = orig_get_spdoinfo
        PreProcessing._get_SettingDataTemplates = orig_get_settingdatatemplates
        PreProcessing._get_Channels = orig_get_channels
        PreProcessing._get_ModuleFunctions = orig_get_module_functions
        PreProcessing._get_ModuleSettings_SettingsGroups = \
            orig_get_modulesettings_settingsgroups
        PreProcessing._resolve_datatypes_from_template_references = \
            orig__resolve_datatype
        PreProcessing._link_channelsettings_into_module_settings = \
            orig_link_channelsettings
        PreProcessing._link_channels_into_spdo_transports = \
            orig_link_channels_into_spdo
        PreProcessing._get_used_channels = orig_get_used_channels

    ##
    # \brief Tests _get_SPDOInfo() for expected results
    def test_get_SPDOInfo(self):
        from pysodb.osddprocessing.preprocessing import PreProcessing

        processingdata = MagicDummy()
        processingdata.spdotransport_output = []
        processingdata.spdotransport_input = []

        inputtransport = MagicDummy()
        inputtransport.Name = 'SafeIN'
        inputtransport.Direction = 'Input'
        inputtransport.OctetSize = 27

        outputtransport = MagicDummy ()
        outputtransport.Name = 'SafeOUT'
        outputtransport.Direction = 'Output'
        outputtransport.OctetSize = 29

        processingdata._channelconfiguration.DataTransport.SPDOTransport = [
            inputtransport, outputtransport]

        obj = PreProcessing(processingdata)
        obj._get_SPDOInfo()

        for item in processingdata.spdotransport_input:
            self.assertIsInstance(item, SPDOTransport)
            self.assertEqual(item.name, 'SafeIN')
            self.assertEqual(item.direction, 'Input')
            self.assertEqual(item.octetsize, 27)

        for item in processingdata.spdotransport_output:
            self.assertIsInstance(item, SPDOTransport)
            self.assertEqual(item.name, 'SafeOUT')
            self.assertEqual(item.direction, 'Output')
            self.assertEqual(item.octetsize, 29)

        processingdata.spdotransport_output = []
        processingdata.spdotransport_input = []

        processingdata._channelconfiguration.DataTransport.SPDOTransport = []
        obj._get_SPDOInfo()
        self.assertEqual(processingdata.spdotransport_input, [])
        self.assertEqual(processingdata.spdotransport_output, [])

    ##
    # \brief Tests _get_SettingDataTemplates() for expected results
    def test_get_SettingDataTemplates(self):
        from pysodb.osddprocessing.preprocessing import PreProcessing

        class SimpleDummy(object):
            pass

        processingdata = MagicDummy()
        processingdata.settingdatatemplates = []

        sdt = SimpleDummy()
        processingdata._designconfiguration.SettingDataTemplates = sdt

        obj = PreProcessing(processingdata)
        obj._get_SettingDataTemplates()

        self.assertEqual(processingdata.settingdatatemplates, [])

        sdt = MagicDummy()
        sdt.Choice = None
        sdt.Range = None
        sdt.ParameterConstants = None
        processingdata._designconfiguration.SettingDataTemplates = sdt

        obj = PreProcessing(processingdata)
        obj._get_SettingDataTemplates()

        self.assertEqual(processingdata.settingdatatemplates, [])

        sdt = MagicDummy()
        sdt.Choice = []
        sdt.Range = []
        sdt.ParameterConstants = []
        processingdata._designconfiguration.SettingDataTemplates = sdt

        obj = PreProcessing(processingdata)
        obj._get_SettingDataTemplates()

        self.assertEqual(processingdata.settingdatatemplates, [])

        expected_items = [SimpleSDTDummy('Choice1', 'DataType1'),
                          SimpleSDTDummy('Choice2', 'DataType2'),
                          SimpleSDTDummy('Range1', 'DataType3'),
                          SimpleSDTDummy('Range2', 'DataType4'),
                          SimpleSDTDummy('PConstant1', 'DataType5'),
                          SimpleSDTDummy('PConstant2', 'DataType6')]

        sdt = MagicDummy()
        sdt.Choice = expected_items[0:2]
        sdt.Range = expected_items[2:4]
        sdt.ParameterConstants.ParameterConstant = expected_items[4:]
        processingdata._designconfiguration.SettingDataTemplates = sdt

        obj = PreProcessing(processingdata)
        obj._get_SettingDataTemplates()

        self.assertEqual(len(processingdata.settingdatatemplates),
                         len(sdt.Choice) + len(sdt.Range) +
                         len(sdt.ParameterConstants.ParameterConstant))

        id_type_list = [(item.id, item.datatype) for item in
                        processingdata.settingdatatemplates]

        for item in expected_items:
            self.assertIn((item.id, item.Type), id_type_list)

    ##
    # \brief Tests _get_Channels() for expected results
    def test_get_Channels(self):

        processingdata = MagicDummy()
        processingdata._channelconfiguration.ChannelGroup = []

        orig_get_sorted_settings_list = \
            pysodb.osddprocessing.preprocessing.get_sorted_settings_list
        pysodb.osddprocessing.preprocessing.get_sorted_settings_list = \
            dummy_get_sorted_settings_list

        from pysodb.osddprocessing.preprocessing import PreProcessing

        processingdata.channelgroups = []
        obj = PreProcessing(processingdata)
        obj._get_Channels()

        self.assertEqual(processingdata.channelgroups, [])

        channelgroup1 = MagicDummy()
        channelgroup1.id = 'channelgroup1id'
        channel1 = MagicDummy()
        channel1.id = 'channel1id'
        channel1.ActiveInFunction = None
        channel1.ChannelSettings = None
        c1transport = MagicDummy()
        c1transport.Name = 'c1transportName'
        c1transport.Index = 'c1TransportIndex'
        c1transport.BitMask = 'c1transportBitMask'
        channel1.DataValue.Transport = c1transport;

        channel1.DataValue.Type = 'channel1Type'

        c1sodpos = MagicDummy()
        c1sodpos.Index = 0x1234
        c1sodpos.SubIndex = 0x12
        channel1.DataValue.SODPosition = c1sodpos
        channelgroup1.Channel = [channel1]

        channelgroup2 = MagicDummy()
        channelgroup2.id = 'channelgroup2id'

        channel2 = MagicDummy()
        channel2.id = 'channel2id'
        c2func = MagicDummy()
        c2func.id = 'c2funcid'
        channel2.ActiveInFunction.FunctionReference = [c2func]
        channel2.ChannelSettings = None
        channel2.DataValue.Transport = None
        channel2.DataValue.SODPosition = None
        channel2.DataValue.Type = 'channel2Type'

        channel3 = MagicDummy()
        channel3.id = 'channel3id'
        channel3.ActiveInFunction = None
        channel3.DataValue.Transport = None
        channel3.DataValue.SODPosition = None
        channel3.DataValue.Type = 'channel3Type'

        c3setting1 = MagicDummy()
        c3setting1.id = 'c3setting1'
        c3setting2 = MagicDummy()
        c3setting2.id = 'c3setting2'

        channel3.ChannelSettings.Setting = [c3setting1, c3setting2]

        channelgroup2.Channel = [channel2, channel3]

        channelgroup3 = MagicDummy()
        channelgroup3.id = 'channelgroup3id'

        channelgroup3.Channel = []

        processingdata.channelgroups = []
        processingdata._channelconfiguration.ChannelGroup = [
            channelgroup1, channelgroup2, channelgroup3]
        obj._get_Channels()

        expected_values = [('channelgroup1id', [
                                {'id': 'channel1id',
                                 'datatype':'channel1Type',
                                 'transportname':'c1transportName',
                                 'transportindex': 'c1TransportIndex',
                                 'transportbitmask':'c1transportBitMask',
                                 'sodindex':0x1234,
                                 'sodsubindex':0x12,
                                 'activeinfunctions' : [],
                                 'settings':[]}
                                                ]
                            ),
                           ('channelgroup2id', [
                                {'id': 'channel2id',
                                 'datatype':'channel2Type',
                                 'transportname': None,
                                 'transportindex': None,
                                 'transportbitmask':None,
                                 'sodindex':None,
                                 'sodsubindex':None,
                                 'activeinfunctions' : ['c2funcid'],
                                 'settings':[]},

                                {'id': 'channel3id',
                                 'datatype':'channel3Type',
                                 'transportname': None,
                                 'transportindex': None,
                                 'transportbitmask': None,
                                 'sodindex': None,
                                 'sodsubindex': None,
                                 'activeinfunctions' : [],
                                 'settings':[c3setting1, c3setting2]}
                                                ]
                            ),
                           ('channelgroup3id', [])]

        for i in range(len(expected_values)):
            self.assertEqual(processingdata.channelgroups[i].id,
                             expected_values[i][0])

            for k in range(len(expected_values[i][1])):
                expected_channel_attr = expected_values[i][1][k]
                channelobj = processingdata.channelgroups[i].channels[k]

                for item in expected_channel_attr.items():
                    key = item [0]
                    value = item [1]
                    self.assertEqual(getattr(channelobj, key), value)

        pysodb.osddprocessing.preprocessing.get_sorted_settings_list = \
            orig_get_sorted_settings_list

    ##
    # \brief Tests _get_ModuleFunctions() for expected results
    def test_get_ModuleFunctions(self):

        processingdata = MagicDummy()
        processingdata._modulefunctions = None
        processingdata.functions = None

        from pysodb.osddprocessing.preprocessing import PreProcessing

        obj = PreProcessing(processingdata)
        obj._get_ModuleFunctions()

        self.assertEqual(processingdata.functions, [])

        processingdata._modulefunctions = MagicDummy()
        processingdata._modulefunctions.Function = []
        obj._get_ModuleFunctions()
        self.assertEqual(processingdata.functions, [])

        rootfunc1 = MagicDummy()
        rootfunc1.id = 'RootFunc1'
        r1func1 = MagicDummy()
        r1func1.Function = []
        r1func1.id = 'RootFunc1/Func1'
        r1func2 = MagicDummy()
        r1func2.id = 'RootFunc1/Func2'
        r1func2.Function = []

        rootfunc1.Function = [r1func1, r1func2]

        rootfunc2 = MagicDummy()
        rootfunc2.id = 'RootFunc2'
        r2func1 = MagicDummy()
        r2func1.id = 'RootFunc2/Func1'
        r2func1.Function = []
        rootfunc2.Function = [r2func1]

        functions = [rootfunc1, rootfunc2]

        processingdata._modulefunctions.Function = functions

        obj._get_ModuleFunctions()

        self.assertEqual(len(processingdata.functions), len(functions))
        self.assertEqual((processingdata.functions[0].id,
                            len(processingdata.functions[0].subfunctions)),
                         ('RootFunc1', 2))

        self.assertEqual(
            (processingdata.functions[0].subfunctions[0].id,
                len(processingdata.functions[0].subfunctions[0].subfunctions)),
            ('RootFunc1/Func1', 0))
        self.assertEqual(
            (processingdata.functions[0].subfunctions[1].id,
                len(processingdata.functions[0].subfunctions[1].subfunctions)),
            ('RootFunc1/Func2', 0))

        self.assertEqual(
            (processingdata.functions[1].id,
                len(processingdata.functions[1].subfunctions)),
            ('RootFunc2', 1))

        self.assertEqual(
            (processingdata.functions[1].subfunctions[0].id,
                len(processingdata.functions[1].subfunctions[0].subfunctions)),
            ('RootFunc2/Func1', 0))

    ##
    # \brief Tests _get_ModuleSettings_SettingsGroups() for expected results
    def test_get_ModuleSettings_SettingsGroups(self):

        processingdata = MagicDummy()

        settingsgroup1 = MagicDummy()
        settingsgroup1.CopyFromChannel = None
        settingsgroup1.id = 'sg1id'
        sg1setting1 = MagicDummy()
        sg1setting2 = MagicDummy()
        settingsgroup1.Setting = [sg1setting1, sg1setting2]

        settingsgroup2 = MagicDummy()
        settingsgroup2.CopyFromChannel = 'none'
        settingsgroup2.id = 'sg2id'
        sg2setting1 = MagicDummy()
        settingsgroup2.Setting = [sg2setting1]

        settingsgroup3 = MagicDummy()
        settingsgroup3.CopyFromChannel = 'Channelgroup1/Channel1'
        settingsgroup3.id = 'sg3id'
        settingsgroup3.Setting = []

        settingsgroups = [settingsgroup1, settingsgroup2, settingsgroup3]
        expected_values = [('sg1id', settingsgroup1.Setting, None),
                           ('sg2id', settingsgroup2.Setting, None),
                           ('sg3id', None, 'Channelgroup1/Channel1')]

        processingdata._designconfiguration.ModuleSettings.SettingsGroup = \
            settingsgroups

        orig_get_sorted_settings_list = \
            pysodb.osddprocessing.preprocessing.get_sorted_settings_list
        pysodb.osddprocessing.preprocessing.get_sorted_settings_list = \
            dummy_get_sorted_settings_list

        from pysodb.osddprocessing.preprocessing import PreProcessing

        processingdata.settingsgroups = []

        obj = PreProcessing(processingdata)
        obj._get_ModuleSettings_SettingsGroups()

        self.assertTrue(hasattr(processingdata, 'settingsgroups'))
        self.assertEqual(len(processingdata.settingsgroups),
                         len(settingsgroups))

        for i in range(len(expected_values)):
            self.assertEqual(processingdata.settingsgroups[i].id,
                             expected_values[i][0])
            self.assertEqual(processingdata.settingsgroups[i].settings,
                             expected_values[i][1])
            self.assertEqual(processingdata.settingsgroups[i].copyfromchannel,
                             expected_values[i][2])

        pysodb.osddprocessing.preprocessing.get_sorted_settings_list = \
            orig_get_sorted_settings_list

    ##
    # \brief Tests _resolve_datatypes_from_template_references() for expected
    # results
    def test_resolve_datatypes_from_template_references(self):
        processingdata = MagicDummy()

        template1 = MagicDummy()
        template1.id = 'Template1'
        template1.datatype = 't1datatype'

        template2 = MagicDummy()
        template2.id = 'Template2'
        template2.datatype = 't2datatype'

        template3 = MagicDummy()
        template3.id = 'Template3'
        template3.datatype = 't3datatype'

        processingdata.settingdatatemplates = [template1, template2, template3]

        channelgroup = MagicDummy()
        channel1 = MagicDummy()

        c1setting1 = MagicDummy()
        c1setting1.templateid = 'Template1'

        c1setting2 = MagicDummy()
        c1setting2.templateid = 'Template2'
        channel1.settings = [c1setting1, c1setting2]

        channel2 = MagicDummy()
        c2setting1 = MagicDummy()
        c2setting1.datatype = 'c2setting1type'
        c2setting1.templateid = None
        channel2.settings = [c2setting1]

        channelgroup.channels = [channel1, channel2]

        channelgroup2 = MagicDummy()
        channel3 = MagicDummy()
        c3setting1 = MagicDummy()
        c3setting1.templateid = 'Template3'
        channel3.settings = [c3setting1]

        channelgroup2.channels = [channel3]

        processingdata.channelgroups = [channelgroup, channelgroup2]

        from pysodb.osddprocessing.preprocessing import PreProcessing
        obj = PreProcessing(processingdata)
        obj._resolve_datatypes_from_template_references()

        self.assertEqual(c1setting1.datatype, template1.datatype)
        self.assertEqual(c1setting2.datatype, template2.datatype)
        self.assertEqual(c2setting1.datatype, 'c2setting1type')
        self.assertEqual(c3setting1.datatype, template3.datatype)

    ##
    # \brief Tests _link_channelsettings_into_module_settings for expected
    # results
    def test_link_channelsettings_into_module_settings(self):

        processingdata = MagicDummy()

        settingsgroup1 = MagicDummy()
        sg1setting1 = MagicDummy()
        settingsgroup1.settings = [sg1setting1]
        settingsgroup1.copyfromchannel = None

        settingsgroup2 = MagicDummy()
        settingsgroup2.copyfromchannel = 'Channelgroup1/Channel1'

        settingsgroup3 = MagicDummy()
        settingsgroup3.copyfromchannel = 'Channelgroup2/Channel2'

        processingdata.settingsgroups = [settingsgroup1,
                                         settingsgroup2,
                                         settingsgroup3]

        channelgroup1 = MagicDummy()
        channelgroup1.id = 'Channelgroup1'

        channelgroup2 = MagicDummy()
        channelgroup2.id = 'Channelgroup2'

        channelgroup3 = MagicDummy()
        channelgroup3.id = 'Channelgroup3'


        channel1 = MagicDummy()
        channel1.id = 'Channel1'

        c1setting1 = MagicDummy()
        c1setting2 = MagicDummy()

        channel1.settings = [c1setting1, c1setting2]

        channel2 = MagicDummy()
        channel2.id = 'Channel2'

        c2setting1 = MagicDummy()

        channel2.settings = [c2setting1]

        channelgroup1.channels = [channel1]
        channelgroup2.channels = [channel2]

        processingdata.channelgroups = [channelgroup1,
                                        channelgroup2,
                                        channelgroup3]

        from pysodb.osddprocessing.preprocessing import PreProcessing
        obj = PreProcessing(processingdata)

        obj._link_channelsettings_into_module_settings()

        self.assertListEqual(processingdata.settingsgroups[0].settings,
                             [sg1setting1])
        self.assertListEqual(processingdata.settingsgroups[1].settings,
                             channel1.settings)
        self.assertListEqual(processingdata.settingsgroups[2].settings,
                             channel2.settings)

    ##
    # \brief Tests _link_channels_into_spdo_transports for expected results
    def test_link_channels_into_spdo_transports(self):
        processingdata = MagicDummy()

        processingdata.spdotransport_input = []
        processingdata.spdotransport_output = []

        inputtransport = MagicDummy()
        inputtransport.name = 'SafeIN'

        processingdata.spdotransport_input.append(inputtransport)

        in_channel1 = MagicDummy()
        in_channel1.transportname = 'SafeIN'

        in_channel2 = MagicDummy()
        in_channel2.transportname = 'SafeIN'

        outputtransport = MagicDummy ()
        outputtransport.name = 'SafeOUT'

        processingdata.spdotransport_output.append(outputtransport)

        from pysodb.osddprocessing.preprocessing import PreProcessing

        processingdata.channelgroups = []

        obj = PreProcessing(processingdata)
        obj._link_channels_into_spdo_transports()
        self.assertListEqual(inputtransport.channels, [])
        self.assertListEqual(outputtransport.channels, [])

        out_channel1 = MagicDummy()
        out_channel1.transportname = 'SafeOUT'

        out_channel2 = MagicDummy()
        out_channel2.transportname = 'SafeOUT'

        out_channel3 = MagicDummy()
        out_channel3.transportname = 'SafeOUT'

        channelgroup1 = MagicDummy()
        channelgroup1.channels = [in_channel1, out_channel1]

        channelgroup2 = MagicDummy()
        channelgroup2.channels = [in_channel2, out_channel2, out_channel3]

        channelgroups = [channelgroup1, channelgroup2]
        processingdata.channelgroups = channelgroups

        obj._link_channels_into_spdo_transports()

        self.assertListEqual(inputtransport.channels,
                             [in_channel1, in_channel2])
        self.assertListEqual(outputtransport.channels,
                             [out_channel1, out_channel2, out_channel3])

    ##
    # \brief Tests _get_used_channels() for expected results
    def test_get_used_channels(self):
        processingdata = MagicDummy()

        processingdata.spdotransport_input = []
        processingdata.spdotransport_output = []

        from pysodb.osddprocessing.preprocessing import PreProcessing

        obj = PreProcessing(processingdata)
        obj._get_used_channels()
        self.assertEqual(processingdata.used_channels, 0)

        inputtransport = MagicDummy()
        inputtransport.Name = 'SafeIN'
        inputtransport.Direction = 'Input'
        inputtransport.OctetSize = 27
        in_channel1 = MagicDummy()
        in_channel2 = MagicDummy()
        inputtransport.channels = [in_channel1, in_channel2]

        processingdata.spdotransport_input.append(inputtransport)

        outputtransport = MagicDummy ()
        outputtransport.Name = 'SafeOUT'
        outputtransport.Direction = 'Output'
        outputtransport.OctetSize = 29
        out_channel1 = MagicDummy()
        out_channel2 = MagicDummy()
        out_channel3 = MagicDummy()
        outputtransport.channels = [out_channel1, out_channel2, out_channel3]

        processingdata.spdotransport_input.append(outputtransport)

        obj._get_used_channels()
        self.assertEqual(processingdata.used_channels,
                         len(inputtransport.channels) +
                         len(outputtransport.channels))

if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
