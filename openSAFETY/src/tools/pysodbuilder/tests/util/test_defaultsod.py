##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_defaultsod.py
# Unit test for the module pysodb.util.defaultsod

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

from pysodb.util.sod_data import SODEntry, SODEntryEnd, SODEntryNumEntries


##
# \brief Tests the class SODDefaultEntries
class Test_SODDefaultEntries(unittest.TestCase):

    ##
    # \brief Test the return of en SODEntryEnd object
    def test_get_end_entry(self):
        from pysodb.util.defaultsod import SODDefaultEntries

        returnvalue = SODDefaultEntries.get_end_entry()
        self.assertTrue(isinstance(returnvalue, SODEntryEnd))

    ##
    # \brief Tests the method get_default_sodentries_from_to() for
    # expected types and SOD indexes of SODEntry objects
    def test_get_default_sodentries_from_to(self):

        from pysodb.util.defaultsod import SODDefaultEntries

        returnvalue = SODDefaultEntries.get_default_sodentries_from_to(
            0x0000, 0x0000, 0, False)

        self.assertEqual(len(returnvalue), 0)
        self.assertEqual(returnvalue, [])

        returnvalue = SODDefaultEntries.get_default_sodentries_from_to(
            0x0000, 0x0000, 0, True)

        self.assertEqual(len(returnvalue), 1)
        self.assertIsInstance(returnvalue[0], SODEntryEnd)

        returnvalue = SODDefaultEntries.get_default_sodentries_from_to(
            0x1C00, 0x1C00, 10, True)

        self.assertEqual(len(returnvalue), 5)

        expected_sodentry_types = [SODEntryNumEntries, SODEntry, SODEntry,
                                   SODEntry, SODEntryEnd]

        for i in range(len(returnvalue)):
            self.assertIsInstance(returnvalue[i], expected_sodentry_types[i])
            if not isinstance(returnvalue[i], SODEntryEnd):
                self.assertEqual(returnvalue[i].objectindex, 0x1C0A)

    ##
    # \brief Tests the method get_default_sodentries_for_object() for
    # expected types and SOD indexes of SODEntry objects
    def test_get_default_sodentries_for_object(self):
        dummy = MagicDummy()
        dummy.returnvalue = MagicDummy()

        from pysodb.util.defaultsod import SODDefaultEntries
        orig_get_entries = SODDefaultEntries.get_default_sodentries_from_to
        SODDefaultEntries.get_default_sodentries_from_to = dummy

        dummy.returnvalue = 42

        sodobjectrange = MagicDummy()
        sodobjectrange.start = 0x4567
        sodobjectrange.end = 0x6789

        returnvalue = SODDefaultEntries.get_default_sodentries_for_object(
                        sodobjectrange, 0, False)
        self.assertEqual(returnvalue, 42)

        self.assertEqual(dummy.get_function_call_params(),
                     [((0x4567, 0x6789, 0, False), {})])

        SODDefaultEntries.get_default_sodentries_from_to = orig_get_entries

if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
