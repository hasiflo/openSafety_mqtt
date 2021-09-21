##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_util.py
# Unit test for the module pysodb.core.codegenerator

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
from tests.common import OutputCatcher, print_wrapper, debug_print
from pysodb.core.codegenerator import Languages, CodegeneratorLanguage, \
    CodeGeneratorBase, get_codegenerator_class, CCodeGenerator


##
# \brief Tests the pysodb.core.codegenerator module
class Test_codegenerator(unittest.TestCase):

    ##
    # \brief Tests the Languages placeholder class
    def test_Languages(self):
        # test, if known CodeGenerator is in Languages class
        self.assertTrue(hasattr(Languages, 'C'))

    ##
    # \brief Tests get_codegenerator() for expected returnvalues and
    # behaviour on non existent code generator (languages)
    def test_get_codegenerator_class(self):
        self.assertTrue(get_codegenerator_class('C'), CCodeGenerator)

        with self.assertRaises(AttributeError):
            get_codegenerator_class('NoLang42')

        @CodegeneratorLanguage('B0')
        class B0CodeGenerator(CodeGeneratorBase):
            pass

        @CodegeneratorLanguage('B1')
        class B1CodeGenerator(CodeGeneratorBase):
            pass

        self.assertTrue(hasattr(Languages, 'B0'))
        self.assertTrue(issubclass(get_codegenerator_class('B0'),
                                   CodeGeneratorBase))
        self.assertTrue(issubclass(get_codegenerator_class('B0'),
                                   B0CodeGenerator))
        self.assertNotIsInstance(get_codegenerator_class('B0'),
                                    B1CodeGenerator)

        self.assertTrue(hasattr(Languages, 'B1'))
        self.assertTrue(issubclass(get_codegenerator_class('B1'),
                                   CodeGeneratorBase))
        self.assertTrue(issubclass(get_codegenerator_class('B1'),
                                   B1CodeGenerator))
        self.assertNotIsInstance(get_codegenerator_class('B1'),
                                    B0CodeGenerator)

    ##
    # \brief Tests the validation of Language names / identifiers
    def test_CodegeneratorLanguage(self):
        CodegeneratorLanguage('Unused')

        with self.assertRaises(SyntaxError):
            invalid = CodegeneratorLanguage ('1243')

    ##
    # \brief Tets the execution of the CodegeneratorLanguage decorator class
    def test_CodegeneratorLanguage_call(self):
        x = CodegeneratorLanguage('X')
        y = CodegeneratorLanguage('Y')
        class XCodeGenerator(CodeGeneratorBase):
            pass
        class YCodeGenerator(CodeGeneratorBase):
            pass

        self.assertFalse(hasattr(Languages, 'X'))
        self.assertFalse(hasattr(Languages, 'Y'))

        y(XCodeGenerator)
        x(YCodeGenerator)

        self.assertTrue(hasattr(Languages, 'X'))
        self.assertTrue(hasattr(Languages, 'Y'))

        new_x = CodegeneratorLanguage('X')

        with self.assertRaises(TypeError):
            new_x(XCodeGenerator)

        class NoCodeGenerator(object):
            pass

        z = CodegeneratorLanguage('Z')

        self.assertFalse(hasattr(Languages, 'Z'))
        with self.assertRaises(TypeError):
            z(NoCodeGenerator)

        self.assertFalse(hasattr(Languages, 'Z'))

    ##
    # \brief Tests the decoration of Codegenerators with Codegenerator Language
    # class
    def test_CodegeneratorLanguage_decorator(self):

        @CodegeneratorLanguage('A')
        class ACodeGenerator(CodeGeneratorBase):
            pass

        self.assertTrue(hasattr(Languages, 'A'))

        with self.assertRaises(SyntaxError):
            @CodegeneratorLanguage('123')
            class OneTwoThreeCodeGenerator(CodeGeneratorBase):
                pass

        self.assertFalse(hasattr(Languages, '123'))

        with self.assertRaises(TypeError):
            @CodegeneratorLanguage('A')
            class AACodeGenerator(CodeGeneratorBase):
                pass

        with self.assertRaises(TypeError):
            @CodegeneratorLanguage('AAA')
            class AAACodeGenerator(object):
                pass
        self.assertFalse(hasattr(Languages, 'AAA'))

    ##
    # \brief Tests the CodeGeneratorBase class / its initialisation
    def test_CodeGeneratorBase(self):

        cgb = CodeGeneratorBase()
        self.assertEqual(cgb.outfuncs, set([]))
        self.assertEqual(cgb.linelength, 80)
        self.assertEqual(cgb.indentWithTab, False)
        self.assertEqual(cgb.indentWithWhitespace, True)

        cgb = CodeGeneratorBase([debug_print, print_wrapper], 100)
        self.assertEqual(cgb.outfuncs, set([debug_print, print_wrapper]))
        self.assertEqual(cgb.linelength, 100)
        self.assertEqual(cgb.indentWithTab, False)
        self.assertEqual(cgb.indentWithWhitespace, True)

    ##
    # \brief Tests the outfunc() method
    def test_CodeGenerator_Base_outfunc(self):

        cgb = CodeGeneratorBase()
        cgb.addoutfunc(debug_print)
        cgb.addoutfunc(OutputCatcher.out)

        OutputCatcher.read()

        teststring = 'This is a test of CodeGeneratorBase.outfunc'

        cgb.outfunc(teststring)

        self.assertTrue(OutputCatcher.lines == 1)
        self.assertEqual(OutputCatcher.read()[0], teststring)

    ##
    # \brief Tests the addition of new output functions
    def test_CodeGeneratorBase_addoutfunc(self):

        cgb = CodeGeneratorBase()
        cgb.addoutfunc(debug_print)
        self.assertTrue(debug_print in cgb.outfuncs)
        self.assertEqual(len(cgb.outfuncs), 1)

        # should do noting, since outfuncs is a set, no exception raised
        cgb.addoutfunc(debug_print)
        self.assertTrue(debug_print in cgb.outfuncs)
        self.assertEqual(len(cgb.outfuncs), 1)

    ##
    # \brief Tests the removal of output functions
    def test_CodeGeneratorBase_removeoutfunc(self):

        cgb = CodeGeneratorBase()
        cgb.addoutfunc(debug_print)
        self.assertTrue(debug_print in cgb.outfuncs)
        self.assertEqual(len(cgb.outfuncs), 1)

        # should do noting, since outfuncs is a set, no exception raised
        cgb.addoutfunc(print_wrapper)
        self.assertTrue(debug_print in cgb.outfuncs)
        self.assertTrue(print_wrapper in cgb.outfuncs)
        self.assertEqual(len(cgb.outfuncs), 2)

        cgb.removeoutfunc(debug_print)
        self.assertFalse(debug_print in cgb.outfuncs)
        self.assertTrue(print_wrapper in cgb.outfuncs)
        self.assertEqual(len(cgb.outfuncs), 1)

        # remove function, which is not in outfuncs set
        with self.assertRaises(KeyError):
            cgb.removeoutfunc(debug_print)

        self.assertFalse(debug_print in cgb.outfuncs)
        self.assertTrue(print_wrapper in cgb.outfuncs)
        self.assertEqual(len(cgb.outfuncs), 1)

    ##
    # \brief Tests object instantiation / initialisation of the CCodeGenerator
    # class
    def test_CCodeGenerator(self):
        ccg = CCodeGenerator()
        self.assertEqual(ccg.whitespaceIndent, 4)

    ##
    # \brief Tests indentation functionality
    def test_CCodeGenerator_getTab_indentation_settings(self):
        ccg = CCodeGenerator()

        ccg.setIndentWithTab()
        self.assertEqual(ccg.indentWithTab, True)
        self.assertEqual(ccg.indentWithWhitespace, False)
        self.assertEqual(ccg.getTab(), '\t')

        ccg.setIndentWithWhitespace()
        self.assertEqual(ccg.indentWithTab, False)
        self.assertEqual(ccg.indentWithWhitespace, True)
        self.assertEqual(ccg.getTab(), ' ' * ccg.whitespaceIndent)

        ccg.setWhitespaceIndentation(10)
        self.assertEqual(ccg.getTab(), ' ' * 10)

    ##
    # \brief Tests new line style settings
    def test_CCodeGenerator_set_newline_style(self):
        ccg = CCodeGenerator()
        ccg.set_newline_style('\r\n')
        self.assertTrue(ccg.newLine, '\r\n')

        ccg.set_newline_style('\n')
        self.assertTrue(ccg.newLine, '\n')

    ##
    # \brief Tests the generatorfunctions of the CCodeGenerator class
    def test_CCodeGenerator_generatorfunctions(self):

        ccg = CCodeGenerator([print_wrapper, OutputCatcher.out])
        OutputCatcher.read()

        ccg.gen_definition('CONSTANT_1', '10')
        self.assertTrue(OutputCatcher.lines == 1)
        self.assertEqual(OutputCatcher.read()[0], '#define CONSTANT_1 10')

        ccg.gen_definition_with_comment('CONSTANT_2', '20', '/* comment */')
        self.assertTrue(OutputCatcher.lines == 1)
        self.assertEqual(OutputCatcher.read()[0],
                         '#define CONSTANT_2 20 /* comment */')

        ccg.gen_comment('This is a comment')
        self.assertTrue(OutputCatcher.lines == 1)
        self.assertEqual(OutputCatcher.read()[0], '/* This is a comment */')

        code = 'printf("Hello World! \r\n");"'
        ccg.gen_rawCode(code)
        self.assertTrue(OutputCatcher.lines == 1)
        self.assertEqual(OutputCatcher.read()[0], code)

        ccg.gen_comment_sectionheader('HEADER')
        self.assertTrue(OutputCatcher.lines == 3)
        caught_lines = OutputCatcher.read()
        self.assertEqual(caught_lines[0], ''.join(
                                    ['/*', '*' * (ccg.linelength - 2)]))
        self.assertEqual(caught_lines[1], ''.join(
                                    [' **', ccg.getTab(), 'HEADER']))
        self.assertEqual(caught_lines[2], ''.join(
                                    [' ', '*' * (ccg.linelength - 3), '*/']))

        # doxygen comment tests
        ccg.gen_comment_doxygen_begin()
        self.assertTrue(OutputCatcher.lines == 1)
        self.assertEqual(OutputCatcher.read()[0], '/**')

        ccg.gen_comment_doxygen_end()
        self.assertTrue(OutputCatcher.lines == 1)
        self.assertEqual(OutputCatcher.read()[0], ' */')

        ccg.gen_comment_doxygen_content('Testcontent')
        self.assertTrue(OutputCatcher.lines == 1)
        self.assertEqual(OutputCatcher.read()[0], ' * Testcontent')

        multilines = ['Line1', 'Line2', 'Line3']
        ccg.gen_comment_multiline_doxygen(multilines)
        self.assertTrue(OutputCatcher.lines == 5)

        readlines = OutputCatcher.read()
        self.assertEqual(readlines[0], '/**')
        for i in range (1, 4):
            self.assertEqual(readlines[i],
                             ''.join([' * ', multilines[i - 1]]))
        self.assertEqual(readlines[4], ' */')

        ccg.gen_comment_inline_doxygen('InlineDoxygen')
        self.assertTrue(OutputCatcher.lines == 1)
        self.assertEqual(OutputCatcher.read()[0], '/** InlineDoxygen */')

        returnvalue = ccg.get_comment_inline_doxygen('getInlineDoxygen')
        self.assertEqual(returnvalue, '/** getInlineDoxygen */')

        ccg.gen_comment_inline_doxygen_member('InlineDoxygen')
        self.assertTrue(OutputCatcher.lines == 1)
        self.assertEqual(OutputCatcher.read()[0], '/**< InlineDoxygen */')

        returnvalue = ccg.get_comment_inline_doxygen_member(
                                                    'getInlineDoxygenMember')
        self.assertEqual(returnvalue, '/**< getInlineDoxygenMember */')

if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
