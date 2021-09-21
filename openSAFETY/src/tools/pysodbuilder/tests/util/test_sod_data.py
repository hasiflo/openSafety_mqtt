##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_sod_data.py
# Unit test for the module pysodb.util.sod_data

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
from tests.common import check_expected_attributes, DummySODEntry, MagicDummy

import pysodb.util.sod_data

from pysodb.util.sod_data import SODAttr, SODEntry, \
    SODEntryNumEntries, SODEntryEnd, Variable, \
    SODRange, SODDataTypes, SODAttributes, SODIndexComments, SODDefines, \
    SODCallback, SODIndexRange, SODStandardObjects


##
# \brief Dummy function for imitating a sort function
def dummy_sort(objectinstance, *args, **kwargs):
    pass


##
# \brief Helper function for cheking an object, its attributes and expected
# values
def check_object(testcase, object_to_test,
                 expected_attributes, expected_sodattributes):

    for item in expected_attributes.items():
        key = item[0]
        value = item [1]
        testcase.assertTrue(hasattr(object_to_test, key))
        attr = getattr(object_to_test, key)
        if isinstance(attr, SODAttr):
            for sodattr in expected_sodattributes.items():
                sodattrkey = sodattr[0]
                sodattrvalue = sodattr[1]
                testcase.assertTrue(hasattr(object_to_test.attributes,
                                            sodattrkey))
                sodattribute = getattr(object_to_test.attributes, sodattrkey)
                testcase.assertEqual(sodattribute, sodattrvalue)
        else:
            testcase.assertEqual(attr, value)


##
# \brief Tests the class SODEntriesList
class Test_SODEntriesList(unittest.TestCase):

    ##
    # \brief Tests the object initialisation and the expected
    # object state / result
    def test_object_initialisation(self):
        from pysodb.util.sod_data import SODEntriesList

        added_entries = []

        obj = SODEntriesList()
        self.assertTrue(isinstance(obj, list))
        self.assertTrue(isinstance(obj, SODEntriesList))

        sodentry = DummySODEntry(0x2345, 0x01)
        added_entries.append(sodentry)
        obj.append(sodentry)

        sodentry = DummySODEntry(0x1234, 0x00)
        added_entries.append(sodentry)
        obj.append(sodentry)

        sodentry = DummySODEntry(0x1234, 0x01)
        added_entries.append(sodentry)
        obj.append(sodentry)

        self.assertEqual(obj, added_entries)

    ##
    # \brief Tests the get_entries_from_to() method
    # for expected results
    def test_get_entries_from_to(self):
        from pysodb.util.sod_data import SODEntriesList
        obj = SODEntriesList()

        result = obj.get_entries_from_to(0x0000, 0x0000)
        self.assertEqual(result, [])

        result = obj.get_entries_from_to(0x0000, 0x0000, True)
        self.assertEqual(result, [])

        result = obj.get_entries_from_to(-10 , -1)
        self.assertEqual(result, [])

        result = obj.get_entries_from_to(0x0000, 0xFFFF)
        self.assertEqual(result, [])

        added_entries = [SODEntry(0x2345, 0x01, None, None, None, None),
                         SODEntry(0x1234, 0x00, None, None, None, None),
                         SODEntry(0x1234, 0x01, None, None, None, None)]

        expected_result = sorted(added_entries)

        obj.extend(added_entries)

        # test test must succeed, as the sort function is overwritten in
        # setUp()
        result = obj.get_entries_from_to(0x0000, 0xFFFF)
        self.assertEqual(result, expected_result)

        result = obj.get_entries_from_to(0x0000, 0x2000)
        self.assertEqual(result, expected_result[:-1])

        result = obj.get_entries_from_to(0x0000, 0x2000)
        self.assertEqual(result, expected_result[:-1], True)

        # test with SODEntryEnd
        end_entry = SODEntryEnd()
        expected_result.append(end_entry)
        obj.append(end_entry)
        result = obj.get_entries_from_to(0x0000, 0x2000, True)
        self.assertEqual(result, [entry for entry in expected_result
                                  if entry.objectindex is None
                                  or entry.objectindex < 0x2000])

        result = obj.get_entries_from_to(0x0000, 0x2000)
        self.assertEqual(result, expected_result[:-2])

    ##
    # \brief Tests get_entries_for_object for expected results
    def test_get_entries_for_object(self):
        from pysodb.util.sod_data import SODEntriesList
        obj = SODEntriesList()

        sodobjectrange = MagicDummy()
        sodobjectrange.start = 0x0000
        sodobjectrange.end = 0x0000

        result = obj.get_entries_for_object(sodobjectrange)
        self.assertEqual(result, [])


        sodobjectrange.start = -10
        sodobjectrange.end = -1

        result = obj.get_entries_for_object(sodobjectrange)
        self.assertEqual(result, [])

        sodobjectrange.start = 0x0000
        sodobjectrange.end = 0xFFFF

        result = obj.get_entries_for_object(sodobjectrange)
        self.assertEqual(result, [])

        added_entries = [SODEntry(0x2345, 0x01, None, None, None, None),
                         SODEntry(0x1234, 0x00, None, None, None, None),
                         SODEntry(0x1234, 0x01, None, None, None, None)]

        expected_result = sorted(added_entries)

        obj.extend(added_entries)

        # test test must succeed, as the sort function is overwritten in
        # setUp()
        result = obj.get_entries_for_object(sodobjectrange)
        self.assertEqual(result, expected_result)

        sodobjectrange.start = 0x0000
        sodobjectrange.end = 0x2000

        result = obj.get_entries_for_object(sodobjectrange)
        self.assertEqual(result, expected_result[:-1])

        # test with SODEntryEnd
        end_entry = SODEntryEnd()
        expected_result.append(end_entry)
        obj.append(end_entry)
        result = obj.get_entries_for_object(sodobjectrange, True)
        self.assertEqual(result, [entry for entry in expected_result
                                  if entry.objectindex is None
                                  or entry.objectindex < 0x2000])

        result = obj.get_entries_for_object(sodobjectrange)
        self.assertEqual(result, expected_result[:-2])


##
# \brief Tests the class SODEntriesList with pached sorting functionality
class Test_SODEntriesList_no_sort(unittest.TestCase):

    def setUp(self):
        self.orig_sort = pysodb.util.sod_data.SODEntriesList.sort
        pysodb.util.sod_data.SODEntriesList.sort = dummy_sort

    def tearDown(self):
        unittest.TestCase.tearDown(self)
        pysodb.util.sod_data.SODEntriesList.sort = self.orig_sort

    ##
    # \brief Tests get_entries_from_to() for expected results
    def test_get_entries_from_to(self):
        from pysodb.util.sod_data import SODEntriesList

        obj = SODEntriesList()

        result = obj.get_entries_from_to(0x0000, 0x0000)
        self.assertEqual(result, [])

        result = obj.get_entries_from_to(0x0000, 0x0000, True)
        self.assertEqual(result, [])

        result = obj.get_entries_from_to(-10 , -1)
        self.assertEqual(result, [])

        result = obj.get_entries_from_to(0x0000, 0xFFFF)
        self.assertEqual(result, [])

        added_entries = [DummySODEntry(0x2345, 0x01),
                         DummySODEntry(0x1234, 0x00),
                         DummySODEntry(0x1234, 0x01)]

        obj.extend(added_entries)

        # test test must succeed, as the sort function is overwritten in
        # setUp()
        result = obj.get_entries_from_to(0x0000, 0xFFFF)
        self.assertEqual(result, added_entries)

        result = obj.get_entries_from_to(0x0000, 0x2000)
        self.assertEqual(result, added_entries[1:])

        result = obj.get_entries_from_to(0x0000, 0x2000)
        self.assertEqual(result, added_entries[1:], True)

        # test with SODEntryEnd
        end_entry = SODEntryEnd()
        added_entries.append(end_entry)
        obj.append(end_entry)
        result = obj.get_entries_from_to(0x0000, 0x2000, True)
        self.assertEqual(result, added_entries[1:])

        result = obj.get_entries_from_to(0x0000, 0x2000)
        self.assertEqual(result, added_entries[1:-1])

    ##
    # \brief Tests get_entries_from_to() for expected results
    def test_get_entries_for_object(self):
        from pysodb.util.sod_data import SODEntriesList
        obj = SODEntriesList()

        sodobjectrange = MagicDummy()
        sodobjectrange.start = 0x0000
        sodobjectrange.end = 0x0000

        result = obj.get_entries_for_object(sodobjectrange)
        self.assertEqual(result, [])


        sodobjectrange.start = -10
        sodobjectrange.end = -1

        result = obj.get_entries_for_object(sodobjectrange)
        self.assertEqual(result, [])

        sodobjectrange.start = 0x0000
        sodobjectrange.end = 0xFFFF

        result = obj.get_entries_for_object(sodobjectrange)
        self.assertEqual(result, [])

        added_entries = [DummySODEntry(0x2345, 0x01),
                         DummySODEntry(0x1234, 0x00),
                         DummySODEntry(0x1234, 0x01)]

        obj.extend(added_entries)

        # test test must succeed, as the sort function is overwritten in
        # setUp()
        result = obj.get_entries_for_object(sodobjectrange)
        self.assertEqual(result, added_entries)

        sodobjectrange.start = 0x0000
        sodobjectrange.end = 0x2000

        result = obj.get_entries_for_object(sodobjectrange)
        self.assertEqual(result, added_entries[1:])

        # test with SODEntryEnd
        end_entry = SODEntryEnd()
        added_entries.append(end_entry)
        obj.append(end_entry)
        result = obj.get_entries_for_object(sodobjectrange, True)
        self.assertEqual(result, added_entries[1:])

        result = obj.get_entries_for_object(sodobjectrange)
        self.assertEqual(result, added_entries[1:-1])


##
# \brief Tests the class SODAttr
class Test_SODAttr(unittest.TestCase):
    expected_attributes = {
        'attributes' : 1,
        'datatype' : 2,
        'max_length' : 3,
        'defaultval_ref' : 4
        }

    ##
    # \brief Tests the object initialisation and expected attributes and values
    def test_SODAttr(self):
        obj = SODAttr(1, 2, 3, 4)
        check_expected_attributes(self, obj, self.expected_attributes)


##
# \brief Tests the class SODEntry
class Test_SODEntry(unittest.TestCase):
    expected_attributes = {
        'objectindex' : 1,
        'subindex' : 2,
        'attributes' : 3,
        'objectdataref' : 4,
        'rangeref' : 5,
        'callback' : 6,
        'comment' : 7,
        'indexalias' : 8,
        'subindexalias' : 9
        }

    expected_attributes2 = {
        'objectindex' : 1,
        'subindex' : 2,
        'attributes' : 3,
        'objectdataref' : 4,
        'rangeref' : 5,
        'callback' : 6,
        'comment' : 7,
        'indexalias' : 8,
        'subindexalias' : None
        }

    expected_attributes3 = {
        'objectindex' : 1,
        'subindex' : 2,
        'attributes' : 3,
        'objectdataref' : 4,
        'rangeref' : 5,
        'callback' : 6,
        'comment' : 7,
        'indexalias' : None,
        'subindexalias' : None
        }

    expected_attributes4 = {
        'objectindex' : 1,
        'subindex' : 2,
        'attributes' : 3,
        'objectdataref' : 4,
        'rangeref' : 5,
        'callback' : 6,
        'comment' : None,
        'indexalias' : None,
        'subindexalias' : None
        }

    ##
    # \brief Tests the object initialisation and expected attributes and values
    def test_SODEntry(self):
        obj = SODEntry(1, 2, 3, 4, 5, 6, 7, 8, 9)
        check_expected_attributes(self, obj, self.expected_attributes)

        obj = SODEntry(1, 2, 3, 4, 5, 6, 7, 8)
        check_expected_attributes(self, obj, self.expected_attributes2)

        obj = SODEntry(1, 2, 3, 4, 5, 6, 7)
        check_expected_attributes(self, obj, self.expected_attributes3)

        obj = SODEntry(1, 2, 3, 4, 5, 6)
        check_expected_attributes(self, obj, self.expected_attributes4)

    ##
    # \brief Tests the operator '<' for expected results
    # with different SODEntry objects and SODEndEntry objecty
    def test_less_than_operator(self):
        entry = SODEntry(0x1234, 0x00, None, None, None, None)
        entry2 = SODEntry(0x1234, 0x01, None, None, None, None)
        entry3 = SODEntry(0x2345, 0x00, None, None, None, None)
        entry4 = SODEntry(None, None, None, None, None, None)
        end = SODEntryEnd()

        self.assertTrue(entry < entry2)
        self.assertTrue(entry < entry3)
        self.assertTrue(entry2 < entry3)

        self.assertTrue(entry < end)
        self.assertTrue(entry2 < end)
        self.assertTrue(entry3 < end)

        self.assertFalse(entry < entry)
        self.assertFalse(entry2 < entry2)
        self.assertFalse(entry3 < entry3)

        self.assertFalse(entry2 < entry)
        self.assertFalse(entry3 < entry)

        self.assertTrue((entry < entry4))

    ##
    # \brief Tests the operator '>' for expected results
    # with different SODEntry objects and SODEndEntry objecty
    def test_greater_than_operator(self):
        entry = SODEntry(0x1234, 0x00, None, None, None, None)
        entry2 = SODEntry(0x1234, 0x01, None, None, None, None)
        entry3 = SODEntry(0x2345, 0x00, None, None, None, None)
        entry4 = SODEntry(None, None, None, None, None, None)
        end = SODEntryEnd()

        self.assertTrue((entry3 > entry2))
        self.assertTrue((entry3 > entry))
        self.assertTrue((entry2 > entry))

        self.assertFalse((entry > end))
        self.assertFalse((entry2 > end))
        self.assertFalse((entry3 > end))

        self.assertFalse((entry > entry))
        self.assertFalse((entry2 > entry2))
        self.assertFalse((entry3 > entry3))

        self.assertFalse((entry > entry4))


##
# \brief Tests the calss SODEntryNumEntries
class Test_SODEntryNumEntries(unittest.TestCase):

    expected_attributes = {
        'objectindex' : 1,
        'subindex' : 2,
        'attributes' : SODAttr([SODAttributes.CONS], SODDataTypes.U8,
                               None, None),
        'objectdataref' : None,
        'rangeref' : None,
        'callback' : SODCallback.SOD_k_NO_CALLBACK,
        'comment' : 3,
        'indexalias' : None,
        'subindexalias' : None
        }

    expected_sodattributes = {
        'attributes' : [SODAttributes.CONS],
        'datatype' : SODDataTypes.U8,
        'max_length' :  None,
        'defaultval_ref' : None
        }

    expected_attributes2 = {
        'objectindex' : 1,
        'subindex' : 2,
        'attributes' : SODAttr([SODAttributes.CONS], SODDataTypes.U8,
                               None, None),
        'objectdataref' : None,
        'rangeref' : None,
        'callback' : SODCallback.SOD_k_NO_CALLBACK,
        'comment' : None,
        'indexalias' : None,
        'subindexalias' : None
        }

    expected_attributes3 = {
        'objectindex' : 0x1234,
        'subindex' : 0x00,
        'attributes' : SODAttr([SODAttributes.CONS], SODDataTypes.U8,
                               None, None),
        'objectdataref' : 'b_noE_42',
        'rangeref' : None,
        'callback' : SODCallback.SOD_k_NO_CALLBACK,
        'comment' : None,
        'indexalias' : None,
        'subindexalias' : None
        }

    expected_sodattributes2 = {
        'attributes' : [SODAttributes.CONS],
        'datatype' : SODDataTypes.U8,
        'max_length' :  None,
        }

    ##
    # \brief Tests the object initialisation and expected attributes and values
    def test_object_initialisation(self):
        obj = SODEntryNumEntries(1, 2, 3)
        check_object(self, obj, self.expected_attributes,
                     self.expected_sodattributes)

        obj = SODEntryNumEntries(1, 2)
        check_object(self, obj, self.expected_attributes2,
                     self.expected_sodattributes)

    ##
    #\brief Tests set_objectdata_and_defaultval_references_noE()
    # for expected values
    def test_set_objectdata_and_defaultval_references_noE(self):
        obj = SODEntryNumEntries(0x1234, 0x00)
        obj.set_objectdata_and_defaultval_references_noE(42)
        check_object(self, obj, self.expected_attributes3,
                     self.expected_sodattributes2)
        self.assertEqual(obj.attributes.defaultval_ref.value, str(42))
        self.assertEqual(obj.attributes.defaultval_ref.identifier, 'b_noE_42')


##
# \brief Tests the class SODEntryEnd
class Test_SODEntryEnd(unittest.TestCase):

    expected_attributes = {
        'objectindex' : None,
        'subindex' : 0xFF,
        'attributes' : SODAttr(['0'], SODDataTypes.EPLS_k_BOOLEAN, None, None),
        'objectdataref' : None,
        'rangeref' : None,
        'callback' : SODCallback.SOD_k_NO_CALLBACK,
        'comment' : SODIndexComments.EndOfSOD,
        'indexalias' : 'SOD_k_END_OF_THE_OD',
        'subindexalias' : None
        }

    expected_sodattributes = {
        'attributes' : ['0'],
        'datatype' : SODDataTypes.EPLS_k_BOOLEAN,
        'max_length' :  None,
        'defaultval_ref' : None
        }

    ##
    # \brief Tests the object initialisation and expected attributes and values
    def test_object_initialisation(self):
        obj = SODEntryEnd()
        check_object(self, obj, self.expected_attributes,
                     self.expected_sodattributes)

    ##
    # \brief Tests the '<' operator for expected results
    # with different SODEntry objects and SODEndEntry objecty
    def test_less_than_operator(self):
        entry = SODEntry(0x1234, 0x00, None, None, None, None)
        entry2 = SODEntry(0x2345, 0x00, None, None, None, None)
        entry3 = SODEntry(0xFFFF, 0x00, None, None, None, None)
        entry4 = SODEntry(0xFFFF, 0xFF, None, None, None, None)
        end = SODEntryEnd()
        end2 = SODEntryEnd()

        self.assertFalse(end < entry)
        self.assertFalse(end < entry2)
        self.assertFalse(end < entry3)
        self.assertFalse(end < entry4)
        self.assertFalse(end < end)
        self.assertFalse(end < end2)

    ##
    # \brief Tests the '>' operator for expected results
    # with different SODEntry objects and SODEndEntry objecty
    def test_greater_than_operator(self):
        entry = SODEntry(0x1234, 0x00, None, None, None, None)
        entry2 = SODEntry(0x2345, 0x00, None, None, None, None)
        entry3 = SODEntry(0xFFFF, 0x00, None, None, None, None)
        entry4 = SODEntry(0xFFFF, 0xFF, None, None, None, None)
        end = SODEntryEnd()
        end2 = SODEntryEnd()

        self.assertTrue(end > entry)
        self.assertTrue(end > entry2)
        self.assertTrue(end > entry3)
        self.assertTrue(end > entry4)
        self.assertFalse(end > end)
        self.assertFalse(end > end2)


##
# \brief Tests the class Variable
class Test_Variable(unittest.TestCase):

    expected_attributes = {
        'datatype' : 1,
        'identifier' : 2,
        'value' : 3,
        'sektor' : 4,
        'arraysize' : 5,
        'process_later' : 6,
        'used_for_generation' : False
        }

    expected_attributes2 = {
        'datatype' : None,
        'identifier' : None,
        'value' : None,
        'sektor' : None,
        'arraysize' : None,
        'process_later' : None,
        'used_for_generation' : False
        }

    ##
    # \brief Tests object initialisation and expected attributes and values
    def test_object_initialisation(self):
        obj = Variable(1, 2, 3, 4, 5, 6)
        check_expected_attributes(self, obj, self.expected_attributes)

        obj = Variable()
        check_expected_attributes(self, obj, self.expected_attributes2)


##
# \brief Tests the class SODDefaultValue
class Test_SODDefaultValue(unittest.TestCase):

    expected_attributes = {
        'datatype' : 1,
        'identifier' : 2,
        'value' : 3,
        'sektor' : 4,
        'arraysize' : 5,
        'process_later' : 6,
        'used_for_generation' : False
        }

    expected_attributes2 = {
        'datatype' : None,
        'identifier' : None,
        'value' : None,
        'sektor' : None,
        'arraysize' : None,
        'process_later' : None,
        'used_for_generation' : False
        }

    ##
    # \brief Tests object intialisation, expected attributes and values and
    # behaviour
    def test_object_initialisation(self):

        orig_errorhandler = pysodb.util.sod_data.ErrorHandler

        dummyerrorhandler = MagicDummy()
        pysodb.util.sod_data.ErrorHandler = dummyerrorhandler

        from pysodb.util.sod_data import SODDefaultValue

        obj = SODDefaultValue(1, 2, 3, 4, 5, 6)
        check_expected_attributes(self, obj, self.expected_attributes)

        obj = SODDefaultValue()
        check_expected_attributes(self, obj, self.expected_attributes2)

        self.assertEqual(dummyerrorhandler.warning.get_number_of_calls(), 1)

        pysodb.util.sod_data.ErrorHandler = orig_errorhandler


##
# \brief Tests the class SODVariable
class Test_SODVariable(unittest.TestCase):
    expected_attributes = {
        'datatype' : 1,
        'identifier' : 2,
        'value' : 3,
        'sektor' : 4,
        'arraysize' : 5,
        'process_later' : 6,
        'used_for_generation' : False
        }

    expected_attributes2 = {
        'datatype' : None,
        'identifier' : None,
        'value' : None,
        'sektor' : None,
        'arraysize' : None,
        'process_later' : None,
        'used_for_generation' : False
        }

    ##
    # \brief Tests object initialisation, expected attributes and values
    def test_object_initialisation(self):
        obj = Variable(1, 2, 3, 4, 5, 6)
        check_expected_attributes(self, obj, self.expected_attributes)

        obj = Variable()
        check_expected_attributes(self, obj, self.expected_attributes2)


##
# \brief Tests the class SODRange
class Test_SODRange(unittest.TestCase):
    expected_attributes = {
        'datatype' : 1,
        'identifier' : 2,
        'low' : 3,
        'high' : 4
        }

    ##
    # \brief Tests object initialisation, expected attributes and values
    def test_object_initialisation(self):
        obj = SODRange(1, 2, 3, 4)
        check_expected_attributes(self, obj, self.expected_attributes)


##
# \brief Tests the class SODDataTypes
class Test_SODDataTypes(unittest.TestCase):
    expected_attributes = {
        'U8' : 'U8',
        'U16' : 'U16',
        'U32' : 'U32',
        'I8' : 'I8',
        'I16' : 'I16',
        'I32' : 'I32',
        'OCT' : 'OCT',
        'DOM' : 'DOM',
        'EPLS_k_BOOLEAN' : 'EPLS_k_BOOLEAN',
        'B8' : 'B8',
        'I64' : 'I64',
        'U64' : 'U64',
        'R32' : 'R32',
        'R64' : 'R64',
        'VSTR' : 'VSTR'
    }

    ##
    # \brief Tests expected attributes and values of the class
    def test_SODDataTypes(self):
        check_expected_attributes(self, SODDataTypes, self.expected_attributes)


##
# \brief Tets the class SODAttributes
class Test_SODAttributes(unittest.TestCase):
    expected_attributes = {
        'CONS' : 'CONS',
        'RO' : 'RO',
        'RW' : 'RW',
        'WO' : 'WO',
        'CRC' : 'CRC',
        'PDO' : 'PDO',
        'NLEN' : 'NLEN',
        'BEF_RD' : 'BEF_RD',
        'BEF_WR' : 'BEF_WR',
        'AFT_WR' : 'AFT_WR'
    }

    ##
    # \brief Tests the class for expected attributes and their values
    def test_SODAttributes(self):
        check_expected_attributes(self, SODAttributes, self.expected_attributes)


##
# \brief Tests the class SODIndexComments
class Test_SODIndexComments(unittest.TestCase):
    expected_attributes = {
        'InputData',
        'OutputData',
        'TxSpdoMappingParameter',
        'TxSpdoComParameter',
        'RxSpdoMappingParameter',
        'RxSpdoComParameter',
        'ErrorStatistics',
        'ErrorRegister',
        'LifeGuarding',
        'NumberOfRetriesResetGuarding',
        'RefreshIntervalResetGuarding',
        'DeviceVendorInformation',
        'UniqueDeviceId',
        'ParameterDownload',
        'CommonCommunicationParameter',
        'VendorModuleSpecificEntries',
        'EndOfSOD'
    }

    ##
    #\brief Tests the class for expected attributes
    def test_SODAttributes(self):
        for item in self.expected_attributes:
            self.assertTrue(hasattr(SODIndexComments, item))


##
# \brief Tests the class SODDefines
class Test_SODDefines(unittest.TestCase):
    expected_attributes = {
        'NULL' : 'NULL',
        'SAFE_INIT_SEKTOR' : 'SAFE_INIT_SEKTOR',
        'SAFE_NO_INIT_SEKTOR' : 'SAFE_NO_INIT_SEKTOR',
        'SOD_k_END_OF_THE_OD' : 'SOD_k_END_OF_THE_OD'
    }

    ##
    # \brief Tests the class for expected attributes and their values
    def test_SODDefines(self):
        check_expected_attributes(self, SODDefines, self.expected_attributes)


##
# \brief Tests the class SODCallback
class Test_SODCallback(unittest.TestCase):
    expected_attributes = {
        'SPDO_SOD_TxMappPara_CLBK' : 'SPDO_SOD_TxMappPara_CLBK',
        'SPDO_SOD_RxMappPara_CLBK' : 'SPDO_SOD_RxMappPara_CLBK',
        'SAPL_SOD_ParameterSet_CLBK' : 'SAPL_SOD_ParameterSet_CLBK',
        'SHNF_SOD_ConsTimeBase_CLBK' : 'SHNF_SOD_ConsTimeBase_CLBK',
        'SOD_k_NO_CALLBACK' : 'SOD_k_NO_CALLBACK'
    }

    ##
    # \brief Tests the class for expected attributes and their values
    def test_SODCallback(self):
        check_expected_attributes(self, SODCallback, self.expected_attributes)


##
# \Tests the class SODIndexRange
class Test_SODIndexRange(unittest.TestCase):
    expected_attributes = {
        'start' : 1,
        'end' : 2
    }

    expected_attributes2 = {
        'start' : 0x1234,
        'end' : 0x1234
    }

    ##
    # \brief Tests object initialisation, expected attributes and their values
    def test_object_initialisation(self):
        obj = SODIndexRange(1, 2)
        check_expected_attributes(self, obj, self.expected_attributes)

        obj = SODIndexRange(0x1234)
        check_expected_attributes(self, obj, self.expected_attributes2)


##
# \brief Tests the class SODStandardObjects
class Test_SODStandardObjects(unittest.TestCase):
    expected_attributes = {
        'ErrorRegister' : (0x1001,),
        'ManufacturerStatusRegister' : (0x1002,),
        'PredefinedErrorField' : (0x1003,),
        'ErrorStatistics' : (0x1004,),
        'LifeGuarding' : (0x100C,),
        'NumberOfRetriesResetGuarding' : (0x100D,),
        'RefreshIntervalResetGuarding' : (0x100E,),
        'DeviceVendorInformation' : (0x1018,),
        'UniqueDeviceId' : (0x1019,),
        'ParameterDownload' : (0x101A,),
        'SCMSpecificParameters' : (0x101B,),
        'CommonCommunicationParameters' : (0x1200,),
        'SSDOCommunicationParameters' : (0x1201,),
        'SNMTCommunicationParameters' : (0x1202,),
        'RxSPDOCommunicationParameter' : (0x1400, 0x17FE),
        'RxSPDOMappingParameter' : (0x1800, 0x1BFE),
        'TxSPDOCommunicationParameter' : (0x1C00, 0x1FFE),
        'UserParameter' : (0x2800, 0x2FFF),
        'TxSPDOMappingParameter' : (0xC000, 0xC3FE),
        'SadrDviList' : (0xC400, 0xC7FE),
        'AdditionalSadrList' : (0xC801, 0xCBFF),
        'SadrUdidList' : (0xCC01, 0xCFFF),
        'AdditionalParameterList' : (0xE400, 0xE7FE),
        'SODEnd' : (0xFFFF,)
    }

    ##
    # \brief Tests expected attributes and SOD indexes of the class
    def test_SODStandardObjects(self):
        for item in self.expected_attributes.items():
            key = item[0]
            value = item[1]
            self.assertTrue(hasattr(SODStandardObjects, key))
            attr = getattr(SODStandardObjects, key)

            if len(value) == 1:
                self.assertEqual(attr.start, value[0])
                self.assertEqual(attr.end, value[0])
            else:
                self.assertEqual(attr.start, value[0])
                self.assertEqual(attr.end, value[1])


if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
