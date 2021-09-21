##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_common.py
# Unit test for the module pysodb.core.common

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
from pysodb.core.common import SODBinfo, SODBsysexit, DefaultFileExtensions, \
    Constants, SPDOCfg, SPDOCom, SCMCfg, EPLSCfg

expected_EPLScfg = {
    'EPLSCfg' : {
                 'maxInstances' : 0, 'maxPayloadLength' : 0,
                 'errorString' : False, 'errorStatistic' : False,
                 'scm' : False, 'applicationObjects' : False,
                 'scmCfg' : SCMCfg, 'spdoCfg' : SPDOCfg, 'constants' : Constants
                 },
    'Constants' : {'enable' : 1, 'disable' : 0, 'notApplicable' : 2000,
                   'configString' : '"demo-sn-gpio"'},
    'SPDOCfg' : {
                 'frameCopyIntern' : False, 'numLookupTable' : False,
                 'connectionValidBitField' : False,
                 'connectionValidStatistic' : False, 'maxRxSyncedPerTx' : 0,
                 'notAnsweredTr' : 0, 'tx' : SPDOCom, 'rx' : SPDOCom
                 },
    'SPDOCom' : {
                   'maxMapEntries' : 0, 'maxPayloadSize' : 0,
                   'spdosActivatedPerCall' : 0, 'maxSpdo' : 0 ,
                   'maxSpdoSdg' : 0
                   },
    'SCMCfg' : {
                'maxNodes' : 0, 'processedNodesPerCall' : 0,
                'maxSadr' : 0
                }
    }


##
# \brief Tests the module pysodb.core.common
class Test_core_common(unittest.TestCase):

    ##
    # \brief Tests SODBinfo datastructure / class
    def test_SODBinfo(self):
        expected_attributes = ['PROGRAM_NAME', 'LOGGER_NAME']
        for item in expected_attributes:
            self.assertTrue(hasattr(SODBinfo, item))

    ##
    # \brief Tests SODBsysexit datastructure / class
    def test_SODBsysexit(self):
        expected_attributes = {'EXIT_SUCCESS':0, 'EXIT_FAILURE':1}
        for item in expected_attributes.keys():
            self.assertTrue(hasattr(SODBsysexit, item))
            self.assertEqual(getattr(SODBsysexit, item),
                             expected_attributes[item])

    ##
    # \brief Tests DefaultFileExtensions datastructure / class
    def test_DefaultFileExtensions(self):
        expected_attributes = ['EXT_BINDING', 'EXT_SCHEMA', 'EXT_OSDD',
                               'EXT_SETTINGS', 'EXT_COG']
        for item in expected_attributes:
            self.assertTrue(hasattr(DefaultFileExtensions, item))

    ##
    # \brief Tests SCMCfg datastructure / class
    def test_SCMCfg(self):
        scmcfg = SCMCfg()
        for item in expected_EPLScfg['SCMCfg'].keys():
            self.assertTrue(hasattr(scmcfg, item))

    ##
    # \brief Tests SPDOCom datastructure / class
    def test_SPDOCom(self):
        spdocom = SPDOCom()
        for item in expected_EPLScfg['SPDOCom'].keys():
            self.assertTrue(hasattr(spdocom, item))

    ##
    # \brief Tests SPDOCfg datastructure / class
    def test_SPDOCfg(self):
        spdocfg = SPDOCfg()
        for item in expected_EPLScfg['SPDOCfg'].keys():
            self.assertTrue(hasattr(spdocfg, item))

        self.assertIsInstance(spdocfg.rx, SPDOCom)
        self.assertIsInstance(spdocfg.tx, SPDOCom)

    ##
    # \brief Tests Constants datastructure / class
    def test_Constants(self):
        constants = Constants()
        for item in expected_EPLScfg['Constants'].keys():
            self.assertTrue(hasattr(constants, item))

    ##
    # \brief Tests EPLSCfg datastructure / class
    def test_EPLSCfg(self):
        eplscfg = EPLSCfg()
        for item in expected_EPLScfg['EPLSCfg'].keys():
            self.assertTrue(hasattr(eplscfg, item))

        self.assertIsInstance(eplscfg.scmCfg, SCMCfg)
        self.assertIsInstance(eplscfg.spdoCfg, SPDOCfg)
        self.assertIsInstance(eplscfg.constants, Constants)

if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
