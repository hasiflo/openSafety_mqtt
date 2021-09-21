##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_stringfunctions.py
# Unit test for the module pysodb.util.stringfunctions

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
from tests.common import MagicDummy, DummyErrorHandler

import pysodb.util.stringfunctions


##
# \brief Class for imitating the class CodeFormatConstants
class DummyCodeFormatConstants(object):
    INDEX_WIDTH = 123
    SUBINDEX_WIDTH = 456


##
# \brief Tests the function get_hex_string()
class Test_get_hex_string(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)
        self.orig_errorhandler = pysodb.util.stringfunctions.ErrorHandler

    def tearDown(self):
        unittest.TestCase.tearDown(self)
        pysodb.util.stringfunctions.ErrorHandler = self.orig_errorhandler

    ##
    # \brief Tests the function get_hex_string()
    def test_get_hex_string(self):
        from pysodb.util.stringfunctions import get_hex_string

        pysodb.util.stringfunctions.ErrorHandler = DummyErrorHandler()

        valid_values = [0x0000, 0x1234, 0xbeef, '0', '42', '0x4567', '0xFF']
        invalid_values = ['invalid', None, -10, 3.1415]

        expected_values = ['0', '1234', 'BEEF', '0', '42', '4567', 'FF']
        expected_values_minwidth_4 = ['0000', '1234', 'BEEF', '0000', '0042',
                                      '4567', '00FF']
        expected_values_leading_0 = ['0x0000', '0x1234', '0xBEEF', '0x0000',
                                     '0x0042', '0x4567', '0x00FF']
        expected_values_lower_case = ['0x0000', '0x1234', '0xbeef', '0x0000',
                                      '0x0042', '0x4567', '0x00ff']

        for i in range(len(valid_values)):
            returnvalue = get_hex_string(valid_values[i])
            self.assertEqual(returnvalue, expected_values[i])

        for i in range(len(invalid_values)):
            with self.assertRaises(SystemExit):
                returnvalue = get_hex_string(invalid_values[i])

        for i in range(len(valid_values)):
            returnvalue = get_hex_string(valid_values[i], 4)
            self.assertEqual(returnvalue, expected_values_minwidth_4[i])

        for i in range(len(invalid_values)):
            with self.assertRaises(SystemExit):
                returnvalue = get_hex_string(invalid_values[i], 4)

        for i in range(len(valid_values)):
            returnvalue = get_hex_string(valid_values[i], 4, True)
            self.assertEqual(returnvalue, expected_values_leading_0[i])

        for i in range(len(invalid_values)):
            with self.assertRaises(SystemExit):
                returnvalue = get_hex_string(invalid_values[i], 4, True)

        for i in range(len(valid_values)):
            returnvalue = get_hex_string(valid_values[i], 4, True, True)
            self.assertEqual(returnvalue, expected_values_lower_case[i])

        for i in range(len(invalid_values)):
            with self.assertRaises(SystemExit):
                returnvalue = get_hex_string(invalid_values[i], 4, True, True)


##
# \brief Tests the function get_hex_string_leading_0x()
class Test_get_hex_string_leading_0x(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)
        self.orig_func = pysodb.util.stringfunctions.get_hex_string

    def tearDown(self):
        unittest.TestCase.tearDown(self)
        pysodb.util.stringfunctions.get_hex_string = self.orig_func

    ##
    # \brief Tests the function get_hex_string_leading_0x()
    def test_get_hex_string_leading_0x(self):
        dummy = MagicDummy()
        pysodb.util.stringfunctions.get_hex_string = dummy

        from pysodb.util.stringfunctions import get_hex_string_leading_0x

        result = get_hex_string_leading_0x ('value')
        self.assertTrue(dummy.get_number_of_calls(), 1)
        self.assertEqual(dummy.get_function_call_params(),
                         [(('value', 0, True, False), {})])

        dummy.reset()
        result = get_hex_string_leading_0x ('value2', 42, True)
        self.assertTrue(dummy.get_number_of_calls(), 1)
        self.assertEqual(dummy.get_function_call_params(),
                         [(('value2', 42, True, True), {})])

        dummy.reset()
        result = get_hex_string_leading_0x ('value3', 53, False)
        self.assertTrue(dummy.get_number_of_calls(), 1)
        self.assertEqual(dummy.get_function_call_params(),
                         [(('value3', 53, True, False), {})])


##
# \brief Tests the function get_sod_index_string()
class Test_get_sod_index_string(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)

        self.orig_func = pysodb.util.stringfunctions.get_hex_string_leading_0x
        self.orig_constants = pysodb.util.stringfunctions.CodeFormatConstants

    def tearDown(self):
        unittest.TestCase.tearDown(self)

        pysodb.util.stringfunctions.get_hex_string_leading_0x = self.orig_func
        pysodb.util.stringfunctions.CodeFormatConstants = self.orig_constants

    ##
    # \brief Tests the function get_sod_index_string() for expected function
    # calls
    def test_get_sod_index_string(self):

        dummy = MagicDummy()

        pysodb.util.stringfunctions.get_hex_string_leading_0x = dummy
        pysodb.util.stringfunctions.CodeFormatConstants = \
            DummyCodeFormatConstants

        from pysodb.util.stringfunctions import get_sod_index_string

        result = get_sod_index_string ('sodindexvalue')
        self.assertTrue(dummy.get_number_of_calls(), 1)
        self.assertEqual(dummy.get_function_call_params(),
                         [(('sodindexvalue', 123), {})])


##
# \brief Tests the function get_sod_subindex_string()
class Test_get_sod_subindex_string(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)

        self.orig_func = pysodb.util.stringfunctions.get_hex_string_leading_0x
        self.orig_constants = pysodb.util.stringfunctions.CodeFormatConstants

    def tearDown(self):
        unittest.TestCase.tearDown(self)

        pysodb.util.stringfunctions.get_hex_string_leading_0x = self.orig_func
        pysodb.util.stringfunctions.CodeFormatConstants = self.orig_constants

    ##
    # \brief Tests the function get_sod_subindex_string() for expected function
    # calls
    def test_get_sod_subindex_string(self):

        dummy = MagicDummy()

        pysodb.util.stringfunctions.get_hex_string_leading_0x = dummy
        pysodb.util.stringfunctions.CodeFormatConstants = \
            DummyCodeFormatConstants

        from pysodb.util.stringfunctions import get_sod_subindex_string

        result = get_sod_subindex_string ('subindexvalue')
        self.assertTrue(dummy.get_number_of_calls(), 1)
        self.assertEqual(dummy.get_function_call_params(),
                         [(('subindexvalue', 456), {})])


##
# \brief Tests the function get_identifier_index_string()
class Test_get_identifier_index_string(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)

        self.orig_func = pysodb.util.stringfunctions.get_hex_string
        self.orig_constants = pysodb.util.stringfunctions.CodeFormatConstants

    def tearDown(self):
        unittest.TestCase.tearDown(self)

        pysodb.util.stringfunctions.get_hex_string = self.orig_func
        pysodb.util.stringfunctions.CodeFormatConstants = self.orig_constants

    ##
    # \brief Tests the function get_identifier_index_string() for expected
    # function calls
    def test_get_identifier_index_string(self):
        dummy = MagicDummy()

        pysodb.util.stringfunctions.get_hex_string = dummy
        pysodb.util.stringfunctions.CodeFormatConstants = \
            DummyCodeFormatConstants

        from pysodb.util.stringfunctions import get_identifier_index_string

        result = get_identifier_index_string ('identifierindexvalue')
        self.assertTrue(dummy.get_number_of_calls(), 1)
        self.assertEqual(dummy.get_function_call_params(),
                         [(('identifierindexvalue', 123), {})])


##
# \brief Tests the function get_identifier_subindex_string()
class Test_get_identifier_subindex_string(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)

        self.orig_func = pysodb.util.stringfunctions.get_hex_string
        self.orig_constants = pysodb.util.stringfunctions.CodeFormatConstants

    def tearDown(self):
        unittest.TestCase.tearDown(self)

        pysodb.util.stringfunctions.get_hex_string = self.orig_func
        pysodb.util.stringfunctions.CodeFormatConstants = self.orig_constants

    ##
    # \brief Tests the function get_identifier_subindex_string() for expected
    # function calls
    def test_get_identifier_subindex_string(self):

        dummy = MagicDummy()

        pysodb.util.stringfunctions.get_hex_string = dummy
        pysodb.util.stringfunctions.CodeFormatConstants = \
            DummyCodeFormatConstants

        from pysodb.util.stringfunctions import get_identifier_subindex_string

        result = get_identifier_subindex_string ('identifiersubindexvalue')
        self.assertTrue(dummy.get_number_of_calls(), 1)
        self.assertEqual(dummy.get_function_call_params(),
                         [(('identifiersubindexvalue', 456), {})])


##
# \brief Tests the function get_struct_range_identifier()
class Test_get_struct_range_identifier(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)

        self.orig_translation = pysodb.util.stringfunctions.SODDataType_DataWord

    def tearDown(self):
        unittest.TestCase.tearDown(self)

        pysodb.util.stringfunctions.SODDataType_DataWord = self.orig_translation

    ##
    # \brief Tests the function get_struct_range_identifier() for expected
    # function calls and expected return values
    def test_get_struct_range_identifier(self):

        values = [('SODDATATYPE1', 1, 2),
                  ('SODDATATYPE2', 0, 63),
                  ('dw', 1, 65535)]

        expected_values = ['s_rg_SODDATATYPE1_1_2',
                           's_rg_SODDATATYPE2_0_63',
                           's_rg_dw_1_65535']

        dummy = MagicDummy()

        pysodb.util.stringfunctions.SODDataType_DataWord = dummy

        from pysodb.util.stringfunctions import get_struct_range_identifier

        for i in range(len(values)):

            dummy.get_DataWord_string.returnvalue = values[i][0]
            returnvalue = get_struct_range_identifier(values[i][0],
                                                      values[i][1],
                                                      values[i][2])

            self.assertEqual(returnvalue, expected_values[i])
            self.assertEqual(
                dummy.get_DataWord_string.get_function_call_params(),
                [((values[i][0],), {})])
            dummy.reset()


##
# \brief Tests the function get_struct_actual_data_identifier()
class Test_get_struct_actual_data_identifier(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)

        self.orig_identifier_index_func = \
            pysodb.util.stringfunctions.get_identifier_index_string
        self.orig_identifier_subindex_func = \
            pysodb.util.stringfunctions.get_identifier_subindex_string

    def tearDown(self):
        unittest.TestCase.tearDown(self)

        pysodb.util.stringfunctions.get_identifier_index_string = \
            self.orig_identifier_index_func
        pysodb.util.stringfunctions.get_identifier_subindex_string = \
            self.orig_identifier_subindex_func

    ##
    # \brief Tests the function get_struct_actual_data_identifier()
    # for expected function calls and expected return values
    # with passed strings as index / subindex parameters
    def test_get_struct_actual_data_identifier(self):

        values = [('0', '1', '2'),
                  ('0', '0x1234', '0x01'),
                  ('0', '0x6ABC', '0xF0')]

        expected_values = ['s_0_act_1_2',
                           's_0_act_0x1234_0x01',
                           's_0_act_0x6ABC_0xF0']

        dummy_index_string = MagicDummy()
        dummy_subindex_string = MagicDummy()

        pysodb.util.stringfunctions.get_identifier_index_string = \
            dummy_index_string
        pysodb.util.stringfunctions.get_identifier_subindex_string = \
            dummy_subindex_string

        from pysodb.util.stringfunctions import get_struct_actual_data_identifier

        for i in range(len(values)):

            dummy_index_string.returnvalue = values[i][1]
            dummy_subindex_string.returnvalue = values[i][2]
            returnvalue = get_struct_actual_data_identifier(values[i][0],
                                                      values[i][1],
                                                      values[i][2])

            self.assertEqual(returnvalue, expected_values[i])
            self.assertEqual(dummy_index_string.get_function_call_params(),
                         [((values[i][1],), {})])
            self.assertEqual(dummy_subindex_string.get_function_call_params(),
                         [((values[i][2],), {})])
            dummy_index_string.reset()
            dummy_subindex_string.reset()

    ##
    # \brief Tests the function get_struct_actual_data_identifier()
    # for expected function calls and expected return values
    # with passed integers as index / subindex parameters
    def test_get_struct_actual_data_identifier2(self):

        from pysodb.util.stringfunctions import get_struct_actual_data_identifier

        values2 = [('0', 1, 2),
                  ('1', 0x1234, 0x01),
                  ('2', 0x6ABC, 0xF0)]

        expected_values2 = ['s_0_act_0001_02',
                            's_1_act_1234_01',
                            's_2_act_6ABC_F0']

        for i in range(len(values2)):

            returnvalue = get_struct_actual_data_identifier(values2[i][0],
                                                      values2[i][1],
                                                      values2[i][2])

            self.assertEqual(returnvalue, expected_values2[i])


##
# \brief Tests the function get_defval_identifier()
class Test_get_defval_identifier(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)
        self.orig_identifier_index_func = \
            pysodb.util.stringfunctions.get_identifier_index_string
        self.orig_identifier_subindex_func = \
            pysodb.util.stringfunctions.get_identifier_subindex_string
        self.orig_translate = \
            pysodb.util.stringfunctions.SODDataType_DefaultValue_Prefix

    def tearDown(self):
        unittest.TestCase.tearDown(self)
        pysodb.util.stringfunctions.get_identifier_index_string = \
            self.orig_identifier_index_func
        pysodb.util.stringfunctions.get_identifier_subindex_string = \
            self.orig_identifier_subindex_func
        pysodb.util.stringfunctions.SODDataType_DefaultValue_Prefix = \
            self.orig_translate

    ##
    # \brief Tests the function get_defval_identifier() for expected function
    # calls and expected return values with passed strings as index /
    # subindex parameters
    def test_get_defval_identifier(self):

        values = [('0', 'SODDATATYPE0', '1', '2'),
                  ('1', 'SODDATATYPE1', '0x1234', '0x01'),
                  ('2', 'SODDATATYPE2', '0x6ABC', '0xF0')]

        expected_values = ['SODDATATYPE0_0_def_1_2',
                           'SODDATATYPE1_1_def_0x1234_0x01',
                           'SODDATATYPE2_2_def_0x6ABC_0xF0']

        dummy_index_string = MagicDummy()
        dummy_subindex_string = MagicDummy()
        dummy_conversion = MagicDummy()

        pysodb.util.stringfunctions.get_identifier_index_string = \
            dummy_index_string
        pysodb.util.stringfunctions.get_identifier_subindex_string = \
            dummy_subindex_string
        pysodb.util.stringfunctions.SODDataType_DefaultValue_Prefix = \
            dummy_conversion

        from pysodb.util.stringfunctions import get_defval_identifier

        for i in range(len(values)):

            dummy_conversion.get_DataWord_string.returnvalue = values[i][1]
            dummy_index_string.returnvalue = values[i][2]
            dummy_subindex_string.returnvalue = values[i][3]

            returnvalue = get_defval_identifier(values[i][0],
                                                      values[i][1],
                                                      values[i][2],
                                                      values[i][3])

            self.assertEqual(returnvalue, expected_values[i])
            self.assertEqual(
                dummy_conversion.get_DataWord_string.get_function_call_params(),
                [((values[i][1],), {})])
            self.assertEqual(dummy_index_string.get_function_call_params(),
                             [((values[i][2],), {})])
            self.assertEqual(dummy_subindex_string.get_function_call_params(),
                             [((values[i][3],), {})])
            dummy_conversion.reset()
            dummy_index_string.reset()
            dummy_subindex_string.reset()

    ##
    # \brief Tests the function get_defval_identifier() for expected function
    # calls and expected return values with passed integers as index /
    # subindex parameters
    def test_get_defval_identifier2(self):

        values = [('0', 'b', 1, 2),
                  ('1', 'w', 0x1234, 0x01),
                  ('2', 'dw', 0x6ABC, 0xF0)]

        expected_values = ['b_0_def_0001_02',
                            'w_1_def_1234_01',
                            'dw_2_def_6ABC_F0']

        dummy_conversion = MagicDummy()

        pysodb.util.stringfunctions.SODDataType_DefaultValue_Prefix = \
            dummy_conversion

        from pysodb.util.stringfunctions import get_defval_identifier

        for i in range(len(values)):

            dummy_conversion.get_DataWord_string.returnvalue = values[i][1]
            returnvalue = get_defval_identifier(values[i][0],
                                                      values[i][1],
                                                      values[i][2],
                                                      values[i][3])

            self.assertEqual(returnvalue, expected_values[i])


##
# \brief Tests the function get_actval_identifier()
class Test_get_actval_identifier(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)
        self.orig_identifier_index_func = \
            pysodb.util.stringfunctions.get_identifier_index_string
        self.orig_identifier_subindex_func = \
            pysodb.util.stringfunctions.get_identifier_subindex_string
        self.orig_translate = \
            pysodb.util.stringfunctions.SODDataType_ActData_Prefix

    def tearDown(self):
        unittest.TestCase.tearDown(self)
        pysodb.util.stringfunctions.get_identifier_index_string = \
            self.orig_identifier_index_func
        pysodb.util.stringfunctions.get_identifier_subindex_string = \
            self.orig_identifier_subindex_func
        pysodb.util.stringfunctions.SODDataType_ActData_Prefix = \
            self.orig_translate

    ##
    # \brief Tests the function get_actval_identifier() for expected function
    # calls and expected return values with passed strings as index /
    # subindex parameters
    def test_get_actval_identifier(self):
        values = [('0', 'SODDATATYPE0', '1', '2'),
                  ('1', 'SODDATATYPE1', '0x1234', '0x01'),
                  ('2', 'SODDATATYPE2', '0x6ABC', '0xF0')]

        expected_values = ['SODDATATYPE0_0_act_1_2',
                           'SODDATATYPE1_1_act_0x1234_0x01',
                           'SODDATATYPE2_2_act_0x6ABC_0xF0']

        dummy_index_string = MagicDummy()
        dummy_subindex_string = MagicDummy()
        dummy_conversion = MagicDummy()

        pysodb.util.stringfunctions.get_identifier_index_string = \
            dummy_index_string
        pysodb.util.stringfunctions.get_identifier_subindex_string = \
            dummy_subindex_string
        pysodb.util.stringfunctions.SODDataType_ActData_Prefix = \
            dummy_conversion

        from pysodb.util.stringfunctions import get_actval_identifier

        for i in range(len(values)):

            dummy_conversion.get_DataWord_string.returnvalue = values[i][1]
            dummy_index_string.returnvalue = values[i][2]
            dummy_subindex_string.returnvalue = values[i][3]

            returnvalue = get_actval_identifier(values[i][0],
                                                      values[i][1],
                                                      values[i][2],
                                                      values[i][3])

            self.assertEqual(returnvalue, expected_values[i])
            self.assertEqual(
                dummy_conversion.get_DataWord_string.get_function_call_params(),
                [((values[i][1],), {})])
            self.assertEqual(dummy_index_string.get_function_call_params(),
                             [((values[i][2],), {})])
            self.assertEqual(dummy_subindex_string.get_function_call_params(),
                             [((values[i][3],), {})])
            dummy_conversion.reset()
            dummy_index_string.reset()
            dummy_subindex_string.reset()

    ##
    # \brief Tests the function get_actval_identifier() for expected function
    # calls and expected return values with passed integers as index /
    # subindex parameters
    def test_get_actval_identifier2(self):

        values = [('0', 'b', 1, 2),
                  ('1', 'w', 0x1234, 0x01),
                  ('2', 'dw', 0x6ABC, 0xF0)]

        expected_values = ['b_0_act_0001_02',
                            'w_1_act_1234_01',
                            'dw_2_act_6ABC_F0']

        dummy_conversion = MagicDummy()

        pysodb.util.stringfunctions.SODDataType_ActData_Prefix = \
            dummy_conversion

        from pysodb.util.stringfunctions import get_actval_identifier

        for i in range(len(values)):

            dummy_conversion.get_DataWord_string.returnvalue = values[i][1]
            returnvalue = get_actval_identifier(values[i][0],
                                                      values[i][1],
                                                      values[i][2],
                                                      values[i][3])

            self.assertEqual(returnvalue, expected_values[i])

if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
