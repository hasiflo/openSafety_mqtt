##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_codegenutil.py
# Unit test for the module pysodb.codegeneration.codegenutil

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
from tests.common import DummyCodeGenUtilData, print_wrapper, OutputCatcher
from pysodb.codegeneration.codegenutil import Borg, CodeGenUtilData, cgendata, \
    generatorfunc, CodeGenUtil
from pysodb.core.codegenerator import CCodeGenerator
from pysodb.core.versioninfo import SODBVersionInfo
from pysodb.core.common import SODBinfo


##
# \brief Tests for codegenutil Module
class Test_codegenutil(unittest.TestCase):

    ##
    # \brief Tests Borg class
    def test_Borg(self):
        borg_a = Borg()
        borg_b = Borg()

        self.assertIsNot(borg_a, borg_b)

        self.assertTrue(borg_a.__dict__ == borg_b.__dict__)
        self.assertEqual(borg_a.__dict__, borg_b.__dict__)

    ##
    # \brief Tets CodeGenUtilData class
    def test_CodeGenUtilData(self):
        cgud_a = CodeGenUtilData()
        cgud_b = CodeGenUtilData()

        self.assertIsNot(cgud_a, cgud_b)

        self.assertTrue(cgud_a.__dict__ == cgud_b.__dict__)
        self.assertEqual(cgud_a.__dict__, cgud_b.__dict__)

        dummy_cogmodule = ('cogmodule', 1, False)
        dummy_sodb_settings = ('sodbsettings', 2, True)
        dummy_osdd_module = ('osddmodule', 3, None)

        cgud_c = CodeGenUtilData([print_wrapper], dummy_cogmodule,
                                 dummy_sodb_settings, dummy_osdd_module,
                                 'dummyfile', False)

        self.assertIsNot(cgud_a, cgud_c)

        self.assertTrue(cgud_a.__dict__ == cgud_c.__dict__)
        self.assertEqual(cgud_a.__dict__, cgud_c.__dict__)

        self.assertIsNot(cgud_a, cgendata)
        self.assertIs(cgud_c, cgud_c)
        self.assertTrue(cgud_a.__dict__ == cgendata.__dict__)
        self.assertEqual(cgud_a.__dict__, cgendata.__dict__)
        self.assertTrue(cgud_c.__dict__ == cgendata.__dict__)
        self.assertEqual(cgud_c.__dict__, cgendata.__dict__)

    ##
    # \brief Tests generatorfunc function generator
    def test_generatorfunc_decorator(self):

        class Dummy(object):

            def __init__(self):
                self.geninitcalled = False
                self.gendeinitcalled = False
                self.functodecoratecalled = False

            def _geninit(self):
                self.geninitcalled = True

            def _gendeinit(self):
                self.gendeinitcalled = True

            @generatorfunc
            def functodecorate(self):
                self.functodecoratecalled = True

        dummy_obj = Dummy()
        dummy_obj.functodecorate()

        self.assertEqual(dummy_obj.geninitcalled, True)
        self.assertEqual(dummy_obj.gendeinitcalled, True)
        self.assertEqual(dummy_obj.functodecoratecalled, True)


##
# \brief Tests CodeGenUtil class
class Test_CodeGenUtil(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        super(Test_CodeGenUtil, cls).setUpClass()
        cls.codegenutildata = DummyCodeGenUtilData()

    ##
    # \brief Tests CodeGenUtil class instantiation and initialisation
    def test_CodeGenUtil(self):

        with self.assertRaises(AttributeError):
            cgu = CodeGenUtil(self.codegenutildata, 'A')

        cgu = CodeGenUtil(self.codegenutildata, 'C')

        self.assertEqual(cgu.codegenutildata, self.codegenutildata)
        self.assertEqual(cgu.language, 'C')
        self.assertIsInstance(cgu.codegen, CCodeGenerator)

        self.assertEqual(cgu.cogmodule, self.codegenutildata.cogmodule)
        self.assertEqual(cgu.outfuncs, self.codegenutildata.outfuncs)
        self.assertEqual(cgu.osdd_module, self.codegenutildata.osdd_module)
        self.assertEqual(cgu.filename, self.codegenutildata.filename)
        self.assertEqual(cgu.sodb_settings, self.codegenutildata.sodb_settings)

    ##
    # \brief Tests _geninit() and _gendeinit() functions
    def test_CodeGenUtil_geninit_gendeinit(self):
        cgu = CodeGenUtil(self.codegenutildata, 'C')

        self.assertFalse(self.codegenutildata.cogmodule.outl in
                         cgu.codegen.outfuncs)
        cgu._geninit()
        self.assertTrue(self.codegenutildata.cogmodule.outl in
                        cgu.codegen.outfuncs)

        cgu._gendeinit()
        self.assertFalse(self.codegenutildata.cogmodule.outl in
                         cgu.codegen.outfuncs)

    ##
    # \brief Tests generator_info() function
    def test_CodeGenUtil_generator_info(self):
        cgu = CodeGenUtil(self.codegenutildata, 'C')
        cgu.codegen.addoutfunc(OutputCatcher.out)

        cgu.generator_info()
        self.assertTrue(OutputCatcher.lines == 1)
        generated_line = OutputCatcher.read()[0]
        substring_to_find = ' '.join([SODBinfo.PROGRAM_NAME,
            SODBVersionInfo.openSAFETY_VERSION_RELEASE, 'generated file for',
            self.codegenutildata.sodb_settings.constants.configString,
            '| generated on:'])

        self.assertTrue(generated_line.find(substring_to_find) != -1)

    ##
    # \brief Tests get_code_generator() function
    def test_CodeGenUtil_get_code_generator(self):
        import pysodb.codegeneration.codegenutil
        orig_get_codegenerator_class = \
            pysodb.codegeneration.codegenutil.codegenerator.get_codegenerator_class
        pysodb.codegeneration.codegenutil.codegenerator.get_codegenerator_class = \
            lambda x: None
        with self.assertRaises(TypeError):
            cgu = CodeGenUtil(self.codegenutildata, 'C')
        pysodb.codegeneration.codegenutil.codegenerator.get_codegenerator_class = \
            orig_get_codegenerator_class

if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
