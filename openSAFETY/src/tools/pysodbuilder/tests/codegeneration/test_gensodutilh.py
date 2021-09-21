##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_gensodutilh.py
# Unit test for the module pysodb.codegeneration.gensodutilh

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
from tests.common import dummy_generatorfunc_decorator, MagicDummy, \
    OutputCatcher, DummyCodeGenUtilData

import pysodb.codegeneration.codegenutil
from pysodb.util.macro import BitAccessMacro
from pysodb.util.structure import Structmember, Struct

# patching the function decorator, this must be done before anything of the
# package or module is imported for the first time
orig_generatorfunc_decorator = pysodb.codegeneration.codegenutil.generatorfunc
pysodb.codegeneration.codegenutil.generatorfunc = dummy_generatorfunc_decorator

import pysodb.codegeneration.gensodutilh

pysodb.codegeneration.codegenutil.generatorfunc = orig_generatorfunc_decorator


##
# \brief Tests gensodutil module init() and Gensodutilh class initialisation
class Test_gensodutilh(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)
        self.orig_processingdata = \
            pysodb.codegeneration.gensodutilh.OSDD_SOD_ProcessingData
        pysodb.codegeneration.gensodutilh.OSDD_SOD_ProcessingData = MagicDummy

    def tearDown(self):
        unittest.TestCase.tearDown(self)
        pysodb.codegeneration.gensodutilh.OSDD_SOD_ProcessingData = \
            self.orig_processingdata

    ##
    # \brief Tests init() function of gensodutilh
    def test_init(self):
        cgud = MagicDummy()
        cgud.outfuncs = [OutputCatcher.out]
        pysodb.codegeneration.codegenutil.cgendata = cgud

        from pysodb.codegeneration.gensodutilh import init, Gensodutilh

        retval = init()
        self.assertTrue(isinstance(retval, Gensodutilh))
        self.assertEqual(retval.language, 'C')
        self.assertTrue(cgud is retval.codegenutildata)

    ##
    # \brief Tests Gensodutilh class initialisation
    def test_Gensodutilh_init(self):

        cgud = DummyCodeGenUtilData()
        pysodb.codegeneration.codegenutil.cgendata = cgud

        from pysodb.codegeneration.gensodutilh import Gensodutilh

        genobj = Gensodutilh(cgud, 'C')
        self.assertIsInstance(genobj.processingdata, MagicDummy)


##
# \brief Tests Gensodutilh class mehtods
class Test_Gensodutilh_class_methods(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)
        self.orig_processingdata = \
            pysodb.codegeneration.gensodutilh.OSDD_SOD_ProcessingData
        pysodb.codegeneration.gensodutilh.OSDD_SOD_ProcessingData = MagicDummy

        cud = DummyCodeGenUtilData()
        cud.outfuncs = [OutputCatcher.out]

        from pysodb.codegeneration.gensodutilh import Gensodutilh
        self.obj = Gensodutilh(cud, 'C')

    def tearDown(self):
        unittest.TestCase.tearDown(self)
        pysodb.codegeneration.gensodutilh.OSDD_SOD_ProcessingData = \
            self.orig_processingdata

    ##
    # \brief Tests the generation of the USEDCHANNELSIZE define
    def test_usedchannelsize(self):

        self.obj.processingdata.used_channels = 0
        self.obj.usedchannelsize()
        self.assertEqual(OutputCatcher.read(),
                         ['#define USEDCHANNELSIZE 0x00 /**< The total count '\
                          'of SPDO channels */'])

        self.obj.processingdata.used_channels = 15
        self.obj.usedchannelsize()
        self.assertEqual(OutputCatcher.read(),
                         ['#define USEDCHANNELSIZE 0x0F /**< The total count '\
                          'of SPDO channels */'])

    ##
    # \brief Tests the generation of struct type declaration
    def test_settings_structs(self):

        self.obj.processingdata.settings_structs_paramstream = [None]
        self.obj.processingdata.settingsgroups_struct_paramstream = None
        self.obj.processingdata.settings_sodpos_struct = None
        self.obj.processingdata.spdo_data_structs = [None]

        self.obj.settings_structs()
        self.assertEqual(OutputCatcher.read(), [])

        self.obj.settings_structs()
        self.assertEqual(OutputCatcher.read(), [])

        structmember1 = Structmember('UINT8', 'member1', 'comment member1')
        structmember2 = Structmember('INT16', 'member2', 'comment member2')
        struct1 = Struct('tStruct1', [structmember1, structmember2],
                         'struct1', 'commentstruct1')

        self.obj.processingdata.settings_structs_paramstream = [struct1]
        self.obj.processingdata.settingsgroups_struct_paramstream = None
        self.obj.processingdata.settings_sodpos_struct = None
        self.obj.processingdata.spdo_data_structs = [None]

        expected_output = [r'/**',
                           r' * \brief commentstruct1',
                           r' */',
                           r'typedef struct',
                           r'{',
                           r'    UINT8 member1;  /**< comment member1 */',
                           r'    INT16 member2;  /**< comment member2 */',
                           r'} tStruct1;',
                           r'']

        self.obj.settings_structs()
        self.assertEqual(OutputCatcher.read(), expected_output)

    ##
    # \brief Tests the generation of the extern declaration of the SPDO data
    # variables
    def test_extern_struct_instances(self):

        struct1 = MagicDummy()
        struct1.datatype = 'tStruct1'
        struct1.identifier = 'struct1'

        struct2 = MagicDummy()
        struct2.datatype = 'tSPDOData'
        struct2.identifier = 'SPDOData_g'

        self.obj.processingdata.spdo_data_structs = []

        self.obj.extern_struct_instances()
        self.assertEqual(OutputCatcher.read(), [])

        self.obj.processingdata.spdo_data_structs = [struct1, struct2]

        expected_output = ['extern tStruct1 struct1 SAFE_NO_INIT_SEKTOR;',
                           'extern tSPDOData SPDOData_g SAFE_NO_INIT_SEKTOR;']

        self.obj.extern_struct_instances()
        self.assertEqual(OutputCatcher.read(), expected_output)

    ##
    # \brief Tests the generation of C macros for accessing single bits of
    # a variable
    def test_bit_access_macros(self):

        macro1 = BitAccessMacro(
                        'CHANNEL1', 'struct.variable', 0x02, 'unsigned int')
        macro2 = BitAccessMacro(
                        'CHANNEL2', 'struct.variable', 0x04, 'unsigned char')

        self.obj.processingdata.bit_access_macros = []

        self.obj.bit_access_macros()
        self.assertEqual(OutputCatcher.read(), [])

        self.obj.processingdata.bit_access_macros = [macro1, macro2]

        expected_output = [
            '/****************************************************************'\
            '***************',
            ' **    macros for bit access to SPDO data',
            ' ****************************************************************'\
            '**************/',
            '#define READ_CHANNEL1 ((((unsigned int) (struct.variable)) >> 1) '\
            '& 0x01)',
            '#define WRITE_CHANNEL1(VAL) ((struct.variable = (struct.variable '\
            '& (~0x02)) | ((VAL & 0x01) << 1)))',
            '',
            '#define READ_CHANNEL2 ((((unsigned char) (struct.variable)) >> 2)'\
            ' & 0x01)',
            '#define WRITE_CHANNEL2(VAL) ((struct.variable = (struct.variable '\
            '& (~0x04)) | ((VAL & 0x01) << 2)))']

        self.obj.bit_access_macros()
        self.assertEqual(OutputCatcher.read(), expected_output)

if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
