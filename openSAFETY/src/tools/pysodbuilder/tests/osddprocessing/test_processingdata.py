##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_processingdata.py
# Unit test for the module pysodb.osddprocessing.processingdata

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

from pysodb.osddprocessing.processingdata import OSDD_SOD_ProcessingData


##
# \brief Tests the class OSDD_SOD_ProcessingData and its methods
class Test_OSDD_SOD_ProcessingData(unittest.TestCase):

    expected_attributes = {
        'sodb_settings' : MagicDummy,
        'numberofentries' : set(),
        '_channelconfiguration' : MagicDummy,
        '_designconfiguration' : MagicDummy,
        '_modulefunctions' : MagicDummy,
        'spdotransport_output' : [],
        'spdotransport_input' : [],
        'settingdatatemplates' : [],
        'channelgroups' : [],
        'functions' : [],
        'settingsgroups' : [],
        'used_channels' : 0,
        'preprocessing' : MagicDummy,
        'sodentries' : MagicDummy,
        'spdo_data_structs' : [],
        'bit_access_macros' : [],
        'settings_structs_paramstream' : [],
        'settingsgroups_struct_paramstream' : None,
        'settings_sodpos_struct' : None,
        'mainprocessing' : MagicDummy,
        'rangedict' : {},
        'actvalue_structs' : [],
        'variables' : [],
        'postprocessing' : MagicDummy
        }

    def setUp(self):
        unittest.TestCase.setUp(self)

        import pysodb.osddprocessing.processingdata

        pysodb.osddprocessing.processingdata.PreProcessing = MagicDummy
        pysodb.osddprocessing.processingdata.MainProcessing = MagicDummy
        pysodb.osddprocessing.processingdata.PostProcessing = MagicDummy
        pysodb.osddprocessing.processingdata.SODEntriesList = MagicDummy

    ##
    # \brief Tests object initialisation and expected function calls
    def test_object_initialisation(self):
        dummyosdd = MagicDummy()
        dummysodbsettings = MagicDummy()

        obj = OSDD_SOD_ProcessingData(dummyosdd, dummysodbsettings)

        #check expected attributes and values
        for item in self.expected_attributes.items():
            key = item[0]
            value = item[1]
            self.assertTrue(hasattr(obj, key))
            attr = getattr(obj, key)

            #if value is type MagicDummy, check if the object has an attribute
            # with an instance of this class, else check for equality
            if item[1] is MagicDummy:
                self.assertTrue(isinstance(attr, MagicDummy))
            else:
                self.assertEqual(attr, value)

        self.assertEqual(
            obj.preprocessing.do_preprocessing.get_number_of_calls(), 1)
        self.assertEqual(
            obj.mainprocessing.do_mainprocessing.get_number_of_calls(), 1)
        self.assertEqual(
            obj.postprocessing.do_postprocessing.get_number_of_calls(), 1)

if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
