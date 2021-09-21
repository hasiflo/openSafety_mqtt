##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_genappc.py
# Unit test for the module pysodb.codegeneration.genappc

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
import pysodb.codegeneration.codegenutil
from tests.common import dummy_generatorfunc_decorator, DummyCodeGenUtilData, \
    MagicDummy, OutputCatcher
from pysodb.util.structure import Structmember, Struct


# patching the function decorator, this must be done before anything of the
# package or module is imported for the first time
orig_generatorfunc_decorator = pysodb.codegeneration.codegenutil.generatorfunc
pysodb.codegeneration.codegenutil.generatorfunc = dummy_generatorfunc_decorator

import pysodb.codegeneration.genappc

pysodb.codegeneration.codegenutil.generatorfunc = orig_generatorfunc_decorator


##
# \brief Tests genappc module init() and Genappc class initialisation
class Test_genappc(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)
        self.orig_processingdata = \
            pysodb.codegeneration.genappc.OSDD_SOD_ProcessingData
        pysodb.codegeneration.genappc.OSDD_SOD_ProcessingData = MagicDummy

    def tearDown(self):
        unittest.TestCase.tearDown(self)
        pysodb.codegeneration.genappc.OSDD_SOD_ProcessingData = \
            self.orig_processingdata
    ##
    # \brief Tests init() function of genappc
    def test_init(self):
        cgud = MagicDummy()
        cgud.outfuncs = [OutputCatcher.out]
        pysodb.codegeneration.codegenutil.cgendata = cgud

        from pysodb.codegeneration.genappc import init, Genappc

        retval = init()
        self.assertTrue(isinstance(retval, Genappc))
        self.assertEqual(retval.language, 'C')
        self.assertTrue(cgud is retval.codegenutildata)

    ##
    # \brief Tests GenEPLScfgh class initialisation
    def test_Genappc_init(self):

        cud = DummyCodeGenUtilData()
        pysodb.codegeneration.codegenutil.cgendata = cud

        from pysodb.codegeneration.genappc import Genappc

        genobj = Genappc(cud, 'C')
        self.assertIsInstance(genobj.processingdata, MagicDummy)
        self.assertTrue(hasattr(genobj, 'defines'))


##
# \brief Tests Genappc class mehtods
class Test_Genappc_class_methods(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)
        self.orig_processingdata = \
            pysodb.codegeneration.genappc.OSDD_SOD_ProcessingData
        pysodb.codegeneration.genappc.OSDD_SOD_ProcessingData = MagicDummy

        cud = DummyCodeGenUtilData()
        cud.outfuncs = [OutputCatcher.out]

        from pysodb.codegeneration.genappc import Genappc
        self.obj = Genappc(cud, 'C')

    def tearDown(self):
        unittest.TestCase.tearDown(self)
        pysodb.codegeneration.genappc.OSDD_SOD_ProcessingData = \
            self.orig_processingdata

    ##
    # \brief Tests generation of SPDO struct declarations
    def test_spdo_struct_declaration(self):
        structmember1 = Structmember('UINT8', 's1member1', 'comment1', '8')
        structmember2 = Structmember('INT32', 's1member2', 'comment2', '32')
        struct1 = Struct('tStruct1', [structmember1, structmember2],
                         'struct1', 'struct comment1')

        structmember3 = Structmember('UINT16', 's2member1', 'comment1', '16')
        struct2 = Struct('tStruct2', [structmember3],
                         'struct2', 'struct comment2')

        self.obj.processingdata.spdo_data_structs = [struct1, struct2]
        self.obj.spdo_struct_declaration()

        expected_output = [r'tStruct1 struct1 SAFE_NO_INIT_SEKTOR;   '\
                           '/**< struct comment1 */',
                           r'tStruct2 struct2 SAFE_NO_INIT_SEKTOR;   '\
                           '/**< struct comment2 */']

        self.assertEqual(OutputCatcher.read(), expected_output)

    ##
    # \brief Tests generation of code for app_reset()
    def test_app_reset(self):
        structmember1 = Structmember('UINT8', 's1member1', 'comment1', '8')
        structmember2 = Structmember('INT32', 's1member2', 'comment2', '32')
        struct1 = Struct('tStruct1', [structmember1, structmember2],
                         'struct1', 'struct comment1')

        structmember3 = Structmember('UINT16', 's2member1', 'comment1', '16')
        struct2 = Struct('tStruct2', [structmember3],
                         'struct2', 'struct comment2')

        self.obj.processingdata.spdo_data_structs = [struct1, struct2]
        self.obj.app_reset()

        expected_output = ['',
                           'struct1.s1member1 = 0;',
                           'struct1.s1member2 = 0;',
                           '',
                           'struct2.s2member1 = 0;'
                           ]

        self.assertEqual(OutputCatcher.read(), expected_output)

    ##
    # \brief Tests generation of APP_SPDO_NUM constant defines
    def test_app_spdo_num_defines(self):
        self.obj.processingdata.spdotransport_input = [1, 2, 3]
        self.obj.processingdata.spdotransport_output = [1]

        self.obj.app_spdo_num_defines()

        expected_output = [
            '#define APP_SPDO_NUM0 (UINT16)0 /**< Id of the SPDO of the '\
            'application */',
            '#define APP_SPDO_NUM1 (UINT16)1 /**< Id of the SPDO of the '\
            'application */',
            '#define APP_SPDO_NUM2 (UINT16)2 /**< Id of the SPDO of the '\
            'application */']

        self.assertEqual(OutputCatcher.read(), expected_output)

    ##
    # \brief Tests generation of connection valid checks according to
    # APP_SDPO_NUM defines
    def test_connection_valid_checks(self):

        self.obj.defines = []
        self.obj.connection_valid_checks()

        expected_output = ['if()']
        self.assertEqual(OutputCatcher.read(), expected_output)

        self.obj.defines = ['APP_SPDO_NUM0', 'APP_SPDO_NUM1', 'APP_SPDO_NUM2']
        self.obj.connection_valid_checks()

        expected_output = ['if(pfnGetConValid_l(APP_SPDO_NUM0) && '\
                           'pfnGetConValid_l(APP_SPDO_NUM1) && '\
                           'pfnGetConValid_l(APP_SPDO_NUM2))']
        self.assertEqual(OutputCatcher.read(), expected_output)

if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
