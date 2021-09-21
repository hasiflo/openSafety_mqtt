##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_postprocessing.py
# Unit test for the module pysodb.osddprocessing.postprocessing

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
from tests.common import MagicDummy, check_expected_attributes, \
     DummySimpleSODEntry

from pysodb.util.sod_data import SODEntry, SODEntryNumEntries, SODAttr, \
    SODDefaultValue, SODDataTypes, SODRange, SODVariable, SODStandardObjects
from pysodb.util.structure import SOD_ActLenPtrData


##
# \brief Tests the PostProcessing class and its methods
class Test_PostProcessing(unittest.TestCase):

    def test_object_initialisation(self):
        expected_attributes = {
           'processingdata' : 1,
           'instance' : 2
        }

        expected_attributes2 = {
           'processingdata' : 1,
           'instance' : None
        }

        from pysodb.osddprocessing.postprocessing import PostProcessing
        obj = PostProcessing(1, 2)
        check_expected_attributes(self, obj, expected_attributes)

        obj = PostProcessing(1)
        check_expected_attributes(self, obj, expected_attributes2)

    ##
    # \brief  Tests the mehtod do_postprocessing() for expected
    # function calls
    def test_do_postprocessing(self):
        from pysodb.osddprocessing.postprocessing import PostProcessing

        processingdata = MagicDummy()
        processingdata.sodentries = [DummySimpleSODEntry(),
                                     DummySimpleSODEntry(),
                                     DummySimpleSODEntry()]

        obj = PostProcessing(processingdata, 0)

        obj._process_actual_value_instance = MagicDummy()
        obj._process_actual_value_variable_declaration = MagicDummy()
        obj._process_default_value = MagicDummy()
        obj._process_range = MagicDummy()
        obj._process_SODEntryNumEntries = MagicDummy()

        obj.do_postprocessing()

        self.assertEqual(
            obj._process_actual_value_instance.get_number_of_calls(),
            len(processingdata.sodentries))
        self.assertEqual(
            obj._process_actual_value_variable_declaration.get_number_of_calls(),
            len(processingdata.sodentries))
        self.assertEqual(
            obj._process_default_value.get_number_of_calls(),
            len(processingdata.sodentries))
        self.assertEqual(
            obj._process_range.get_number_of_calls(),
            len(processingdata.sodentries))
        self.assertEqual(
            obj._process_SODEntryNumEntries.get_number_of_calls(), 1)

        processingdata.sodentries = [DummySimpleSODEntry()]
        processingdata.variables = [MagicDummy()]
        processingdata.actvalue_structs = [MagicDummy()]
        processingdata.rangedict = {MagicDummy()}
        obj = PostProcessing(processingdata)

        obj._process_actual_value_instance = MagicDummy()
        obj._process_actual_value_variable_declaration = MagicDummy()
        obj._process_default_value = MagicDummy()
        obj._process_range = MagicDummy()
        obj._process_SODEntryNumEntries = MagicDummy()

        obj.do_postprocessing()

        self.assertEqual(
            obj._process_actual_value_instance.get_number_of_calls(),
            len(processingdata.sodentries))
        self.assertEqual(
            obj._process_actual_value_instance.get_function_call_params(),
            [((processingdata.sodentries[0], '?INSTANCE?',
               processingdata.actvalue_structs), {})])

        self.assertEqual(
            obj._process_actual_value_variable_declaration.get_number_of_calls(),
            len(processingdata.sodentries))
        self.assertEqual(
            obj._process_actual_value_variable_declaration.get_function_call_params(),
            [((processingdata.sodentries[0], '?INSTANCE?',
               processingdata.variables), {})])

        self.assertEqual(
            obj._process_default_value.get_number_of_calls(),
            len(processingdata.sodentries))
        self.assertEqual(
            obj._process_default_value.get_function_call_params(),
            [((processingdata.sodentries[0], '?INSTANCE?',
               processingdata.variables), {})])

        self.assertEqual(
            obj._process_range.get_number_of_calls(),
            len(processingdata.sodentries))
        self.assertEqual(
            obj._process_range.get_function_call_params(),
            [((processingdata.sodentries[0],
               processingdata.rangedict), {})])

        self.assertEqual(
            obj._process_SODEntryNumEntries.get_number_of_calls(), 1)
        self.assertEqual(
            obj._process_SODEntryNumEntries.get_function_call_params(),
            [((), {})])

    ##
    # \brief Tests the post processing of SODEntryNumEntries objects
    def test_process_SODEntryNumEntries(self):

        processingdata = MagicDummy()
        processingdata.sodentries = [
            SODEntry(0x1234, 0x01, None, None, None, None),
            SODEntryNumEntries(0x1234, 0x00, None),
            SODEntry(0x1234, 0x02, None, None, None, None)
            ]

        processingdata.variables = []
        processingdata.actvalue_structs = []
        processingdata.rangedict = {}

        from pysodb.osddprocessing.postprocessing import PostProcessing

        obj = PostProcessing(processingdata)
        result = obj._process_SODEntryNumEntries()
        self.assertListEqual(result, [2])

        self.assertEqual(
             processingdata.sodentries[1].objectdataref,
             'b_noE_2')
        self.assertEqual(
            processingdata.sodentries[1].attributes.defaultval_ref.value,
            '2')
        self.assertEqual(
            processingdata.sodentries[1].attributes.defaultval_ref.identifier,
            'b_noE_2')

        processingdata.sodentries = [
            SODEntry(0x1234, 0x01, None, None, None, None),
            SODEntryNumEntries(0x2345, 0x00, None),
            SODEntry(0x1234, 0x02, None, None, None, None)
            ]

        obj = PostProcessing(processingdata)
        result = obj._process_SODEntryNumEntries()
        self.assertListEqual(result, [0])

        self.assertEqual(
            processingdata.sodentries[1].objectdataref, 'b_noE_0')
        self.assertEqual(
            processingdata.sodentries[1].attributes.defaultval_ref.value, '0')
        self.assertEqual(
            processingdata.sodentries[1].attributes.defaultval_ref.identifier,
            'b_noE_0')

        processingdata.sodentries = [
            SODEntry(0x1234, 0x01, None, None, None, None),
            SODEntryNumEntries(0x2345, 0x00, None),
            SODEntryNumEntries(0x1234, 0x00, None),
            SODEntry(0x1234, 0x02, None, None, None, None),
            SODEntry(0x1234, 0x03, None, None, None, None)
            ]

        obj = PostProcessing(processingdata)
        result = obj._process_SODEntryNumEntries()
        self.assertListEqual(result, [0, 3])

        self.assertEqual(
            processingdata.sodentries[1].objectdataref,
            'b_noE_0')
        self.assertEqual(
            processingdata.sodentries[1].attributes.defaultval_ref.value,
            '0')
        self.assertEqual(
            processingdata.sodentries[1].attributes.defaultval_ref.identifier,
            'b_noE_0')

        self.assertEqual(
            processingdata.sodentries[2].objectdataref,
            'b_noE_3')
        self.assertEqual(
            processingdata.sodentries[2].attributes.defaultval_ref.value,
            '3')
        self.assertEqual(
            processingdata.sodentries[2].attributes.defaultval_ref.identifier,
            'b_noE_3')

    ##
    # \brief Tests the post processing of SODDefaultValue objects
    def test_process_default_value(self):
        defval = SODDefaultValue(value='value', process_later=True)
        attr = SODAttr(None, SODDataTypes.U8, 'maxobj_len', defval)
        entry = SODEntry(0x1234, 0x01, attr, None, None, None)

        processingdata = MagicDummy()

        defvallist = []

        from pysodb.osddprocessing.postprocessing import PostProcessing
        obj = PostProcessing(processingdata)

        obj._process_default_value(entry, '0', defvallist)
        self.assertTrue(defvallist[0] == defval)
        self.assertFalse(defval.process_later)
        self.assertEqual(defval.identifier, 'b_0_def_1234_01')
        self.assertEqual(defval.arraysize, None)

        attr.datatype = SODDataTypes.OCT
        defval.process_later = True

        defvallist = []

        obj._process_default_value(entry, 'instance', defvallist)
        self.assertTrue(defvallist[0] == defval)
        self.assertFalse(defval.process_later)
        self.assertEqual(defval.identifier, 'ab_instance_def_1234_01')
        self.assertEqual(defval.arraysize, 'maxobj_len')

    ##
    # \brief Tests the post processing of ranges
    def test_process_range(self):
        defval = SODDefaultValue(value='value', process_later=True)
        attr = SODAttr(None, SODDataTypes.U8, 'maxobj_len', defval)
        entry = SODEntry(0x1234, 0x01, attr, None,
                         SODRange(None, 'RangeIdentifier', 0, 42),
                         None)

        processingdata = MagicDummy()
        rangedict = {}

        from pysodb.osddprocessing.postprocessing import PostProcessing
        obj = PostProcessing(processingdata)
        obj._process_range(entry, rangedict)

        self.assertDictEqual(rangedict, {'s_rg_b_0_42' : entry.rangeref})

    ##
    # \brief Tests the post processing of SOD_ActLenPtrData objects
    def test_process_actual_value_variable_declaration(self):
        sodvariable = SODVariable(process_later=True)
        actlenptrdata = SOD_ActLenPtrData(sodvariable, sektor='Sektor')
        attr = SODAttr(None, SODDataTypes.DOM, 'maxobj_len', None)
        entry = SODEntry(0x1234, 0x01, attr, actlenptrdata, None, None)

        processingdata = MagicDummy()

        from pysodb.osddprocessing.postprocessing import PostProcessing

        variable_declarations = []

        obj = PostProcessing(processingdata)
        obj._process_actual_value_variable_declaration(
                        entry, 'Instance', variable_declarations)
        self.assertListEqual(variable_declarations, [actlenptrdata])
        self.assertEqual(actlenptrdata.ptr_data.initval.arraysize, 'maxobj_len')
        self.assertEqual(actlenptrdata.ptr_data.initval.identifier,
                         'ab_Instance_act_1234_01')

        sodvariable = SODVariable(identifier='VariableIdentifier_?INSTANCE?',
                                  process_later=True)
        actlenptrdata = SOD_ActLenPtrData(sodvariable, sektor='Sektor')
        attr = SODAttr(None, SODDataTypes.U16, 'maxobj_len', None)
        entry = SODEntry(0x1234, 0x01, attr, actlenptrdata, None, None)

        variable_declarations = []

        obj = PostProcessing(processingdata)
        obj._process_actual_value_variable_declaration(entry, '123',
                                                       variable_declarations)
        self.assertListEqual(variable_declarations, [actlenptrdata])
        self.assertEqual(actlenptrdata.ptr_data.initval.arraysize, None)
        self.assertEqual(actlenptrdata.ptr_data.initval.identifier,
                         'VariableIdentifier_123')
    ##
    # \brief Tests the processing of the actual value references of
    # SODEntry objects
    def test_process_actual_value_instance(self):

        sodvariable = SODVariable(process_later=True)
        actlenptrdata = SOD_ActLenPtrData(sodvariable, sektor='Sektor')
        attr = SODAttr(None, SODDataTypes.DOM, 'maxobj_len', None)
        entry = SODEntry(0x1234, 0x01, attr, actlenptrdata, None, None)

        processingdata = MagicDummy()

        structlist = []

        from pysodb.osddprocessing.postprocessing import PostProcessing

        obj = PostProcessing(processingdata)
        obj._process_actual_value_instance(entry, '123', structlist)
        self.assertEqual(structlist, [actlenptrdata])
        self.assertIsInstance(actlenptrdata.ptr_data.initval, SODVariable)
        self.assertEqual(actlenptrdata.act_len.initval, 'maxobj_len')
        self.assertEqual(actlenptrdata.identifier, 's_123_act_1234_01')

        actlenptrdata = SOD_ActLenPtrData('sodvariable_?INSTANCE?',
                                          sektor='Sektor')
        attr = SODAttr(None, SODDataTypes.DOM, 'maxobj_len2', None)
        entry = SODEntry(0x1234, 0x01, attr, actlenptrdata, None, None)

        structlist = []
        obj._process_actual_value_instance(entry, '456', structlist)
        self.assertEqual(structlist, [actlenptrdata])
        self.assertEqual(actlenptrdata.ptr_data.initval, 'sodvariable_456')
        self.assertEqual(actlenptrdata.act_len.initval, 'maxobj_len2')
        self.assertEqual(actlenptrdata.identifier, 's_456_act_1234_01')

    ##
    # \brief Tests the processing of actual value references of type string
    # and the correct replacement of SPDOPOS string placeholders
    # for SODEntry objects.
    def test_process_actual_value_instance2(self):

        attr = SODAttr(None, SODDataTypes.DOM, 'maxobj_len', None)
        entry = SODEntry(0x1234, 0x01, attr,
                         'objectdataref_?INSTANCE?_?SPDOPOS?',
                         None, None)

        processingdata = MagicDummy()

        structlist = []

        from pysodb.osddprocessing.postprocessing import PostProcessing

        obj = PostProcessing(processingdata)
        obj._process_actual_value_instance(entry, '123', structlist)
        self.assertEqual(entry.objectdataref, 'objectdataref_123_?SPDOPOS?')
        self.assertListEqual(structlist, [])

        # Test SODEntry objects with SOD index in and outisde the range
        # of RxSPDOCommunicationParameter indexes
        structlist = []
        attr = SODAttr(None, SODDataTypes.DOM, 'maxobj_len', None)
        entry = SODEntry(
                    SODStandardObjects.RxSPDOCommunicationParameter.start + 1,
                    0x01, attr, 'objectdataref_?INSTANCE?_?SPDOPOS?', None,
                    None)

        obj._process_actual_value_instance(entry, '234', structlist)
        self.assertEqual(entry.objectdataref, 'objectdataref_234_1')
        self.assertListEqual(structlist, [])

        structlist = []
        attr = SODAttr(None, SODDataTypes.DOM, 'maxobj_len', None)
        entry = SODEntry(
                    SODStandardObjects.RxSPDOCommunicationParameter.end + 1,
                    0x01, attr, 'objectdataref_?INSTANCE?_?SPDOPOS?', None,
                    None)

        # Test SODEntry objects with SOD index in and outisde the range
        # of TxSPDOCommunicationParameter indexes
        obj._process_actual_value_instance(entry, '345', structlist)
        self.assertEqual(entry.objectdataref, 'objectdataref_345_?SPDOPOS?')
        self.assertListEqual(structlist, [])

        structlist = []
        attr = SODAttr(None, SODDataTypes.DOM, 'maxobj_len', None)
        entry = SODEntry(
                    SODStandardObjects.TxSPDOCommunicationParameter.start + 2 ,
                    0x01, attr, 'objectdataref_?INSTANCE?_?SPDOPOS?', None,
                    None)

        obj._process_actual_value_instance(entry, '456', structlist)
        self.assertEqual(entry.objectdataref, 'objectdataref_456_2')
        self.assertListEqual(structlist, [])

        structlist = []
        attr = SODAttr(None, SODDataTypes.DOM, 'maxobj_len', None)
        entry = SODEntry(
                    SODStandardObjects.TxSPDOCommunicationParameter.end + 1,
                    0x01, attr, 'objectdataref_?INSTANCE?_?SPDOPOS?', None,
                    None)

        obj._process_actual_value_instance(entry, '567', structlist)
        self.assertEqual(entry.objectdataref, 'objectdataref_567_?SPDOPOS?')
        self.assertListEqual(structlist, [])

if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
