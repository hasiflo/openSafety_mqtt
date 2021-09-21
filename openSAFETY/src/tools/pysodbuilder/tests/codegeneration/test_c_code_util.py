##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_c_code_util.py
# Unit tests for the module pysodb.codegeneration.c_code_util

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
from tests.common import MagicDummy, OutputCatcher

from pysodb.codegeneration.c_code_util import CBitAccessMacroAdapter, \
    to_c_sod_entry, generate_structure
from pysodb.core.codegenerator import CCodeGenerator
from pysodb.util.macro import BitAccessMacro
from pysodb.util.structure import Structmember, Struct, SOD_ActLenPtrData
from pysodb.util.sod_data import SODAttr, SODDefaultValue, SODAttributes, \
    SODDataTypes, SODEntry, SODCallback, SODEntryEnd, SODEntryNumEntries, \
    SODRange


##
# \brief Tests the functions of the module pysodb.codegeneration.c_code_util
class Test_c_code_util_functions(unittest.TestCase):

    # \brief Tests the function to_c_sod_entry for correct
    # code generation
    def test_to_c_sod_entry(self):

        attr = SODAttr([SODAttributes.RW], SODDataTypes.U8, None, None)

        sodentry = SODEntry(0x1234, 0x00, attr, 'stringRef', None,
                            SODCallback.SOD_k_NO_CALLBACK, 'acomment')

        returnval = to_c_sod_entry(sodentry)
        self.assertEqual(returnval, '{0x1234, 0x00, {RW, U8, 0x1UL, NULL}, '\
                         '&stringRef, NULL, SOD_k_NO_CALLBACK},')

        returnval = to_c_sod_entry(sodentry, True)
        self.assertEqual(returnval, '{0x1234, 0x00, {RW, U8, 0x1UL, NULL}, '\
                         '&stringRef, NULL, SOD_k_NO_CALLBACK}')

        defval = SODDefaultValue(value='42', identifier='defval')
        attr = SODAttr([SODAttributes.RW], SODDataTypes.U8, None, defval)
        sodentry = SODEntry(0x1234, 0x00, attr, 'stringRef', None,
                            SODCallback.SOD_k_NO_CALLBACK, 'acomment')

        returnval = to_c_sod_entry(sodentry)
        self.assertEqual(returnval, '{0x1234, 0x00, {RW, U8, 0x1UL, &defval}, '\
                         '&stringRef, NULL, SOD_k_NO_CALLBACK},')

        sodentry = SODEntryEnd()
        returnval = to_c_sod_entry(sodentry, True)
        self.assertEqual(returnval, '{SOD_k_END_OF_THE_OD, 0xFF, '\
                         '{0, EPLS_k_BOOLEAN, 0x01UL, NULL}, NULL, '\
                         'NULL, SOD_k_NO_CALLBACK}')

        actlen_ptrdata = SOD_ActLenPtrData()
        attr = SODAttr([SODAttributes.RW, SODAttributes.CRC],
                       SODDataTypes.DOM, 'arraysize', None)
        sodentry = SODEntry(0x1234, 0x00, attr, actlen_ptrdata, None,
                            SODCallback.SAPL_SOD_ParameterSet_CLBK, 'acomment')
        with self.assertRaises(SystemExit):
            returnval = to_c_sod_entry(sodentry, True)

        actlen_ptrdata = SOD_ActLenPtrData(identifier='actLenPtrDataIdentifier')
        attr = SODAttr([SODAttributes.RW, SODAttributes.CRC],
                       SODDataTypes.DOM, 'arraysize', None)
        sodentry = SODEntry(0x1234, 0x00, attr, actlen_ptrdata, None,
                            SODCallback.SAPL_SOD_ParameterSet_CLBK, 'acomment')
        returnval = to_c_sod_entry(sodentry, True)
        self.assertEqual(returnval, '{0x1234, 0x00, '\
                         '{RW | CRC, DOM, arraysize, NULL}, '\
                         '&actLenPtrDataIdentifier, '\
                         'NULL, SAPL_SOD_ParameterSet_CLBK}')

        arange = SODRange('UINT16', 'RangeIdentifier', 1, 65535)
        attr = SODAttr([SODAttributes.RW], SODDataTypes.U16, None, None)
        sodentry = SODEntry(0x4321, 0x03, attr, 'objectref', arange,
                            SODCallback.SAPL_SOD_ParameterSet_CLBK, 'acomment')
        returnval = to_c_sod_entry(sodentry)
        self.assertEqual(returnval, '{0x4321, 0x03, {RW, U16, 0x2UL, NULL}, '\
                         '&objectref, &RangeIdentifier, '\
                         'SAPL_SOD_ParameterSet_CLBK},')

        attr = SODAttr([SODAttributes.RW], SODDataTypes.U16, None, None)
        sodentry = SODEntry(0x4321, 0x03, attr, 'objectref',
                            None, None, 'acomment')
        returnval = to_c_sod_entry(sodentry)
        self.assertEqual(returnval, '{0x4321, 0x03, {RW, U16, 0x2UL, NULL}, '\
                         '&objectref, NULL, NULL},')

        attr = SODAttr([SODAttributes.RW], SODDataTypes.I32, None, None)
        sodentry = SODEntry(0x4321, 0x03, attr, 'objectref',
                            None, None, 'acomment')
        sodentry.subindexalias = 'subindexConstant'
        returnval = to_c_sod_entry(sodentry)
        self.assertEqual(returnval, '{0x4321, subindexConstant, '\
                         '{RW, I32, 0x4UL, NULL}, &objectref, NULL, NULL},')

        sodentry = SODEntryNumEntries(0x2345, 0x00, 'acomment')
        returnval = to_c_sod_entry(sodentry)
        self.assertEqual(returnval, '{0x2345, 0x00, {CONS, U8, 0x1UL, NULL}, '\
                         'NULL, NULL, SOD_k_NO_CALLBACK},')

    ##
    # \brief Tests the function generate_structure() for correct
    # code generation
    def test_generate_structure(self):

        codegen = CCodeGenerator([OutputCatcher.out])
        structmember1 = Structmember('UINT8', 'identifier1', 'comment1')
        structmember2 = Structmember('INT32', 'identifier2', 'comment2')
        emptystruct = Struct('tEmptyStructType', [],
                             'emptystruct', 'emptystruct comment')

        expected_output = [r'/**',
                           r' * \brief emptystruct comment',
                           r' */',
                           r'typedef struct',
                           r'{',
                           r'} tEmptyStructType;']

        generate_structure(codegen, emptystruct)
        caught_text = OutputCatcher.read()

        self.assertEqual(len(caught_text), len(expected_output))
        for i in range(len(expected_output)):
            self.assertEqual(caught_text[i], expected_output[i])

        struct = Struct('tStructType', [structmember1, structmember2],
                             'struct', 'struct comment')

        expected_output = [r'/**',
                           r' * \brief struct comment',
                           r' */',
                           r'typedef struct',
                           r'{',
                           r'    UINT8 identifier1;  /**< comment1 */',
                           r'    INT32 identifier2;  /**< comment2 */',
                           r'} tStructType;']


        generate_structure(codegen, struct)
        caught_text = OutputCatcher.read()

        self.assertEqual(len(caught_text), len(expected_output))
        for i in range(len(expected_output)):
            self.assertEqual(caught_text[i], expected_output[i])


##
# \brief Tests the class CBitAccessMacroAdapter
class Test_CBitAccessMacroAdapter(unittest.TestCase):

    ##
    # \brief Tests the initialisation of the class instance
    def test_object_initialisation(self):

        dummy = MagicDummy()

        with self.assertRaises(TypeError):
            obj = CBitAccessMacroAdapter(dummy)

        macro = BitAccessMacro('MacroName', 'aVariable', '0x04')

        obj = CBitAccessMacroAdapter(macro)
        self.assertEqual(obj.macro, macro)

    ##
    # \brief Tests the method get_read_bit_macro() for correct code generation
    def test_get_read_bit_macro(self):
        macro = BitAccessMacro('MACRONAME', 'aVariable', '0x04')

        obj = CBitAccessMacroAdapter(macro)

        expected_macro = '#define READ_MACRONAME ((aVariable >> 2) & 0x01)'
        returnval = obj.get_read_bit_macro()

        self.assertEqual(returnval, expected_macro)

        macro = BitAccessMacro('MACRONAME', 'aVariable', '0x04', 'unsigned int')

        obj = CBitAccessMacroAdapter(macro)

        expected_macro = '#define READ_MACRONAME ((((unsigned int) '\
                         '(aVariable)) >> 2) & 0x01)'
        returnval = obj.get_read_bit_macro()

        self.assertEqual(returnval, expected_macro)

    ##
    # \brief Tests the method get_write_bit_macro() for correct code generation
    def test_get_write_bit_macro(self):
        macro = BitAccessMacro('MACRONAME', 'aVariable', 0x08)

        obj = CBitAccessMacroAdapter(macro)

        expected_macro = '#define WRITE_MACRONAME(VAL) ((aVariable = '\
                         '(aVariable & (~0x08)) | ((VAL & 0x01) << 3)))'
        returnval = obj.get_write_bit_macro()

        self.assertEqual(returnval, expected_macro)

if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
