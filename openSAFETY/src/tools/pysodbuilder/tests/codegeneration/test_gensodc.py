##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_gensodc.py
# Unit test for the module pysodb.codegeneration.gensodc

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
from tests.common import OutputCatcher, MagicDummy, \
    dummy_generatorfunc_decorator, DummyCodeGenUtilData

from pysodb.util.sod_data import SODEntry, SODAttr, SODAttributes, \
    SODEntryNumEntries, SODEntryEnd, SODDefaultValue, SODRange, SODCallback, \
    SODDefines, SODVariable, SODDataTypes
from pysodb.util.structure import SOD_ActLenPtrData, Structmember, Struct
import pysodb.codegeneration.codegenutil


# patching the function decorator, this must be done before anything of the
# package or module is imported for the first time
orig_generatorfunc_decorator = pysodb.codegeneration.codegenutil.generatorfunc
pysodb.codegeneration.codegenutil.generatorfunc = dummy_generatorfunc_decorator

import pysodb.codegeneration.gensodc

pysodb.codegeneration.codegenutil.generatorfunc = orig_generatorfunc_decorator


##
# \brief Tests gensodc module init() and Gensodc class initialisation
class Test_Gensodc(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)
        self.orig_processingdata = \
            pysodb.codegeneration.gensodc.OSDD_SOD_ProcessingData
        pysodb.codegeneration.gensodc.OSDD_SOD_ProcessingData = MagicDummy

    def tearDown(self):
        unittest.TestCase.tearDown(self)
        pysodb.codegeneration.gensodc.OSDD_SOD_ProcessingData = \
            self.orig_processingdata

    ##
    # \brief Tests init() function of gensodc
    def test_init(self):
        cgud = MagicDummy()
        cgud.outfuncs = [OutputCatcher.out]
        pysodb.codegeneration.codegenutil.cgendata = cgud

        from pysodb.codegeneration.gensodc import init, Gensodc

        retval = init()
        self.assertTrue(isinstance(retval, Gensodc))
        self.assertEqual(retval.language, 'C')
        self.assertTrue(cgud is retval.codegenutildata)

    ##
    # \brief Tests Gensodc class initialisation
    def test_Gensodc_init(self):

        cud = DummyCodeGenUtilData()
        pysodb.codegeneration.codegenutil.cgendata = cud

        from pysodb.codegeneration.gensodc import Gensodc

        genobj = Gensodc(cud, 'C')
        self.assertIsInstance(genobj.processingdata, MagicDummy)


##
# \brief Tests Genappc class mehtods
class Test_Gensodc_class_methods(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)
        self.orig_processingdata = \
            pysodb.codegeneration.gensodc.OSDD_SOD_ProcessingData
        pysodb.codegeneration.gensodc.OSDD_SOD_ProcessingData = MagicDummy

        cud = DummyCodeGenUtilData()
        cud.outfuncs = [OutputCatcher.out]

        from pysodb.codegeneration.gensodc import Gensodc
        self.obj = Gensodc(cud, 'C')
        OutputCatcher.read()

    def tearDown(self):
        unittest.TestCase.tearDown(self)
        pysodb.codegeneration.gensodc.OSDD_SOD_ProcessingData = \
            self.orig_processingdata

    ##
    # \brief Tests generation of the range defines
    def test_range_definitions(self):
        self.obj.processingdata.rangedict = {}
        self.obj.range_definitions()
        self.assertEqual(OutputCatcher.read(), [])

        #range1 is of a signed type, should be ignored
        range1 = SODRange('I16', 'dw_range_1_65535', 1, 65535)
        range2 = SODRange('U8', 'b_range_0_239', 0, 239)
        range3 = SODRange('U16', 'w_range_10_1023', 10, 1023)

        self.obj.processingdata.rangedict = {range1.identifier:range1,
                                             range2.identifier:range2,
                                             range3.identifier:range3}
        expected_output = [
            'static t_U8_RANGE b_range_0_239 = {0, 239};',
            'static t_U16_RANGE w_range_10_1023 = {10, 1023};']

        self.obj.range_definitions()
        self.assertEqual(OutputCatcher.read(), expected_output)

    ##
    # \brief Tests the generation of the 'number of entries' variable
    # definitions
    def test_number_of_entries(self):
        number_of_entries = []

        self.obj.processingdata.numberofentries = number_of_entries

        self.obj.number_of_entries_definitions()
        self.assertEqual(OutputCatcher.read(), [])

        self.obj.processingdata.numberofentries = [5, 0, 1, 9, 12]
        expected_output = [
            'static UINT8 b_noE_5 SAFE_INIT_SEKTOR = 5;  /**< number of '\
            'entries = 5 */',
            'static UINT8 b_noE_0 SAFE_INIT_SEKTOR = 0;  /**< number of '\
            'entries = 0 */',
            'static UINT8 b_noE_1 SAFE_INIT_SEKTOR = 1;  /**< number of '\
            'entries = 1 */',
            'static UINT8 b_noE_9 SAFE_INIT_SEKTOR = 9;  /**< number of '\
            'entries = 9 */',
            'static UINT8 b_noE_12 SAFE_INIT_SEKTOR = 12;    /**< number of '\
            'entries = 12 */']

        self.obj.number_of_entries_definitions()
        self.assertEqual(OutputCatcher.read(), expected_output)

    ##
    # \brief Tests the generation of the variable declarations for
    # parameter setting struct instances
    def test_paramstream_settings_definition(self):
        self.obj.processingdata.settingsgroups_struct_paramstream = None
        self.obj.paramstream_settings_definition()
        self.assertEqual(OutputCatcher.read(), [])

        s1member1 = Structmember('UINT8', 's1member1', 's1member1 comment',
                                 '0x01')
        s1member2 = Structmember('UINT32', 's1member2', 's1member2 comment',
                                 '0xDEADBEEF')
        settingstruct1 = Struct('tStruct1', [s1member1, s1member2],
                                'struct1', 'struct1 comment')

        self.obj.processingdata.settingsgroups_struct_paramstream = \
            settingstruct1

        expected_output = [
                    'static tStruct1 struct1 = {0x01, 0xDEADBEEF};']

        self.obj.paramstream_settings_definition()
        self.assertEqual(OutputCatcher.read(), expected_output)

    ##
    # \brief Tests the generation of the variable declarations for
    # parameter setting struct instances
    def test_sodpos_settings_definition(self):
        self.obj.processingdata.settings_sodpos_struct = None

        self.obj.sodpos_settings_definition()
        self.assertEqual(OutputCatcher.read(), [])

        s1member1 = Structmember('UINT8', 's1member1', 's1member1 comment',
                                 '0x01')
        s1member2 = Structmember('UINT32', 's1member2', 's1member2 comment',
                                 '0xDEADBEEF')
        settingstruct1 = Struct('tStruct1', [s1member1, s1member2], 'struct1',
                                'struct1 comment')

        self.obj.processingdata.settings_sodpos_struct = settingstruct1

        expected_output = [
                           'static tStruct1 struct1 = {0x01, 0xDEADBEEF};']

        self.obj.sodpos_settings_definition()
        self.assertEqual(OutputCatcher.read(), expected_output)

    ##
    # \brief Test the generation of SOD entries
    def test_make_sod_entries(self):
        self.obj.processingdata.sodentries = []

        self.obj.make_sod_entries()
        self.assertEqual(OutputCatcher.read(), [])

        sodentry1 = SODEntryNumEntries(0x1234, 0x00, 'NumEntry comment')

        attr2 = SODAttr([SODAttributes.CONS, SODAttributes.CRC],
                         'datatype1', 'len1', None)
        sodentry2 = SODEntry(0x1231, 0x01, attr2, None, None, None,
                              'SODEntry2 comment')

        defval3 = SODDefaultValue('UINT8', 'default_val', '0x01')
        attr3 = SODAttr([SODAttributes.PDO], 'U8', None, defval3)
        range3 = SODRange('UINT8', 'range_data', 0, 63)
        sodentry3 = SODEntry(0x1231, 0x02, attr3, 'object_data',
                             range3, SODCallback.SOD_k_NO_CALLBACK,
                             'SODEntry2 comment')

        sodentryend = SODEntryEnd()

        self.obj.processingdata.sodentries = [sodentryend, sodentry1,
                                               sodentry2, sodentry3]
        self.obj.make_sod_entries()

        expected_output = [
            '',
            '/* SODEntry2 comment */',
            '{0x1231, 0x01, {CONS | CRC, datatype1, len1, NULL}, '\
            'NULL, NULL, NULL},',
            '/* SODEntry2 comment */',
            '{0x1231, 0x02, {PDO, U8, 0x1UL, &default_val}, '\
            '&object_data, &range_data, '
            'SOD_k_NO_CALLBACK},',
            '',
            '/* NumEntry comment */',
            '{0x1234, 0x00, {CONS, U8, 0x1UL, NULL}, '\
            'NULL, NULL, SOD_k_NO_CALLBACK},',
            '',
            '/* end of SOD */',
            '{SOD_k_END_OF_THE_OD, 0xFF, {0, EPLS_k_BOOLEAN, 0x01UL, NULL}, '\
            'NULL, NULL, '
            'SOD_k_NO_CALLBACK}'
            ]

        self.assertEqual(OutputCatcher.read(), expected_output)

    ##
    # \brief Test the geeneration of default value variable definitions
    def test_default_values_definitions(self):
        self.obj.processingdata.sodentries = []
        self.obj.default_values_definitions()

        self.assertEqual(OutputCatcher.read(), [])

        sodentry1 = SODEntryNumEntries(0x1234, 0x00, 'Object 0x1234')

        defval2 = SODDefaultValue('UINT8', 'default_val2', '0x00',
                                  SODDefines.SAFE_INIT_SEKTOR, 'arraysize')
        attr2 = SODAttr([SODAttributes.CONS, SODAttributes.CRC],
                        'datatype2', 'len1', defval2)
        actval2 = SOD_ActLenPtrData('data2', 'lenght2', 'actval2')
        sodentry2 = SODEntry(0x1234, 0x01, attr2, actval2, None, None,
                             'SODEntry2 comment')

        defval3 = SODDefaultValue('UINT8', 'default_val3', '0x01')
        attr3 = SODAttr([SODAttributes.PDO], 'U8', None, defval3)
        range3 = SODRange('UINT8', 'range_data', 0, 63)
        sodentry3 = SODEntry(0x1234, 0x02, attr3, 'object_data3',
                             range3, SODCallback.SOD_k_NO_CALLBACK,
                             'SODEntry3 comment')

        defval4 = SODDefaultValue('UINT8', 'default_val4', '0x00',
                                  SODDefines.SAFE_INIT_SEKTOR, 'arraysize4')
        attr4 = SODAttr([SODAttributes.CONS, SODAttributes.CRC],
                        'datatype4', 'len4', defval4)
        var4 = SODVariable('tVar4', 'var4')
        actval4 = SOD_ActLenPtrData(var4, 'lenght4', 'actval4')
        sodentry4 = SODEntry(0x1234, 0x01, attr4, actval4, None, None,
                             'SODEntry4 comment')

        defval5 = SODDefaultValue('INT16', 'default_val5')
        attr5 = SODAttr([SODAttributes.PDO], 'U16', None, defval5)
        sodentry5 = SODEntry(0x2345, 0x00, attr5, 'object_data5', None,
                             SODCallback.SOD_k_NO_CALLBACK, 'SODEntry5 comment')

        sodentry6 = SODEntryNumEntries(0x3456, 0x00, 'Object 0x3456')

        defval7 = SODDefaultValue('UINT8', 'default_val7', '0x00',
                                  SODDefines.SAFE_INIT_SEKTOR,
                                  'default_val_arraysize7')
        attr7 = SODAttr([SODAttributes.CONS, SODAttributes.CRC],
                        SODDataTypes.DOM, 'arraysize7', defval7)
        var7 = SODVariable(datatype=None, identifier='var7',
                           arraysize='var_arraysize7')
        actval7 = SOD_ActLenPtrData(var7, 'length7', 'actval7')
        sodentry7 = SODEntry(0x3456, 0x01, attr7, actval7,
                             None, None, 'SODEntry7 comment')

        defval8 = SODDefaultValue('INT32', 'default_val8', '0xFF00FF00',
                                  SODDefines.SAFE_INIT_SEKTOR,
                                  'default_val_arraysize8')
        attr8 = SODAttr([SODAttributes.CONS, SODAttributes.CRC],
                        SODDataTypes.I32, 'arraysize8', defval8)
        var8 = SODVariable(datatype=None, identifier='var8',
                           arraysize='var_arraysize8')
        actval8 = SOD_ActLenPtrData(var8, 'length8', 'actval8')
        sodentry8 = SODEntry(0x3456, 0x02, attr8, actval8,
                             None, None, 'SODEntry8 comment')

        sodentryend = SODEntryEnd()

        self.obj.processingdata.sodentries = [sodentryend, sodentry1,
                                              sodentry2, sodentry3,
                                              sodentry4, sodentry5,
                                              sodentry6, sodentry7,
                                              sodentry8]
        self.obj.default_values_definitions()

        expected_output = [
            '',
            '/* Object 0x1234 */',
            'static UINT8 default_val2[arraysize] SAFE_INIT_SEKTOR = 0x00;',
            'static tVar4 var4;',
            'static UINT8 default_val4[arraysize4] SAFE_INIT_SEKTOR = 0x00;',
            'static UINT8 default_val3 = 0x01;',
            '',
            '/* SODEntry5 comment */',
            'static INT16 default_val5;',
            '',
            '/* Object 0x3456 */',
            'static UINT8 var7[var_arraysize7];',
            'static UINT8 default_val7[default_val_arraysize7] '\
            'SAFE_INIT_SEKTOR = 0x00;',
            'static INT32 var8[var_arraysize8];',
            'static INT32 default_val8[default_val_arraysize8] '\
            'SAFE_INIT_SEKTOR = '
            '0xFF00FF00;']

        self.assertEqual(OutputCatcher.read(), expected_output)

    ##
    # \brief Tests the generation of variable definitions for SOD_ActLenPtrData
    # objects
    def test_act_data_definitions(self):
        self.obj.processingdata.actvalue_structs = []

        self.obj.act_data_definitions()
        self.assertEqual(OutputCatcher.read(), [])

        actvalue1 = SOD_ActLenPtrData('data', 'length1', 'identifier1')
        actvalue2 = SOD_ActLenPtrData('data2', '0x10', 'identifier2',
                                      SODDefines.SAFE_NO_INIT_SEKTOR)

        actvalue3 = SOD_ActLenPtrData('data3', 'length3', 'identifier3')
        variable = SODVariable('tVariable', 'variable', '0x00',
                               SODDefines.SAFE_INIT_SEKTOR)
        actvalue3.initval[1] = variable

        actvalue4 = SOD_ActLenPtrData('data4', 'length4', 'identifier4')
        variable2 = SODVariable('tVariable2', 'variable2', '0x01',
                                SODDefines.SAFE_INIT_SEKTOR, 'k_arraysize')
        actvalue4.initval[1] = variable2

        self.obj.processingdata.actvalue_structs = [actvalue1, actvalue2,
                                                    actvalue3, actvalue4]

        expected_output = [
            'static SOD_t_ACT_LEN_PTR_DATA identifier1 = {length1, &data};',
            'static SOD_t_ACT_LEN_PTR_DATA identifier2 SAFE_NO_INIT_SEKTOR '\
            '= {0x10, &data2};',
            'static SOD_t_ACT_LEN_PTR_DATA identifier3 = {length3, &variable};',
            'static SOD_t_ACT_LEN_PTR_DATA identifier4 = {length4, '\
            '&variable2[0]};'
            ]

        self.obj.act_data_definitions()
        self.assertEqual(OutputCatcher.read(), expected_output)

        actvalue1.initval = []
        self.obj.processingdata.actvalue_structs = [actvalue1]

        with self.assertRaises(SystemExit):
            self.obj.act_data_definitions()

if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
