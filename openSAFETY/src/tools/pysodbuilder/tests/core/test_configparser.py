##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_configparser.py
# Unit test for the module pysodb.core.configparser

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
import os

# third party packages or modules

# own packages or modules
from tests.common import DummyErrorHandler, current_python_version, python32

from pysodb.core.common import EPLSCfg
import pysodb.core.configparser
from pysodb.core.configparser import SODBuilderConfigParser


# expected data read from pysodbsettings.ini testfile
sections_of_ini_file = {
    'EPLSCfg' : {
                 'maxInstances' : '1', 'maxPayloadLength' : '8',
                 'errorString' : 'OFF', 'errorStatistic' : 'OFF',
                 'scm' : 'False', 'applicationObjects' : 'OFF',
                 'saplReportStateChange':'OFF'
                 },
    'Constants' : {'configString' : '"demo-sn-gpio"'},
    'SPDOCfg' : {
                 'frameCopyIntern' : 'OFF', 'numLookupTable' : 'OFF',
                 'connectionValidBitField' : 'ON',
                 'connectionValidStatistic' : 'OFF', 'maxRxSyncedPerTx' : '1',
                 'notAnsweredTr' : '100', 'propDelayStatistic':'OFF',
                 'spdo40bitCTSupport':'ON', 'spdoExtendedCTBitField':'OFF'
                 },
    'TXSPDOCom' : {
                   'maxMapEntries' : '4', 'maxPayloadSize' : '4',
                   'spdosActivatedPerCall' : '1', 'maxSpdo' : '1' ,
                   'maxSpdoSdg' : '0'
                   },
    'RXSPDOCom' : {
                   'maxMapEntries' : '7', 'maxPayloadSize' : '8',
                   'spdosActivatedPerCall' : '2', 'maxSpdo' : '3',
                   'maxSpdoSdg' : '0'
                   },
    'SCMCfg' : {
                'maxNodes' : '0', 'processedNodesPerCall' : '0',
                'maxSadr' : '0'
                }
    }

##
# \brief Tests the module pysodb.core.configparser
class Test_configparser(unittest.TestCase):

    testdata_path = os.path.normpath(os.path.dirname(os.path.dirname(__file__)))
    testdata_path = os.path.join(testdata_path, 'testdata')

    @classmethod
    def setUpClass(cls):
        super(Test_configparser, cls).setUpClass()
        cls.orig_errorhandler = pysodb.core.errorhandler.ErrorHandler
        pysodb.core.configparser.errorhandler.ErrorHandler = DummyErrorHandler()

    @classmethod
    def tearDownClass(cls):
        super(Test_configparser, cls).tearDownClass()
        pysodb.core.configparser.errorhandler.ErrorHandler = \
            cls.orig_errorhandler

    def setUp(self):
        unittest.TestCase.setUp(self)
        self.configparser = SODBuilderConfigParser()

    ##
    # \brief Tests the reading of the config / settings file and the
    # expected returned data structure
    def test_read_configfile_and_get_settings(self):
        settings_file_path = os.path.join(Test_configparser.testdata_path,
                                          'pysodbsettings.ini')

        with open(settings_file_path, 'r') as f:
            self.configparser.read_configfile(f)
            settings = self.configparser.get_settings()
            self.assertIsNotNone(settings)
            self.assertIsInstance(settings, EPLSCfg)

        # testing pyhton configparser result
        for section in sections_of_ini_file.keys():
            self.assertTrue(section in list(
                                        self.configparser.parser.sections()))

        # testing read data
        for section in sections_of_ini_file:
            if (section == 'EPLSCfg'):
                for key in sections_of_ini_file[section]:
                    self.assertTrue(hasattr(settings, key))
                    self.assertEqual(getattr(settings, key),
                                     sections_of_ini_file[section][key])

            elif (section == 'Constants'):
                for key in sections_of_ini_file[section]:
                    self.assertTrue(hasattr(settings.constants, key))
                    self.assertEqual(getattr(settings.constants, key),
                                     sections_of_ini_file[section][key])

            elif (section == 'SPDOCfg'):
                for key in sections_of_ini_file[section]:
                    self.assertTrue(hasattr(settings.spdoCfg, key))
                    self.assertEqual(getattr(settings.spdoCfg, key),
                                     sections_of_ini_file[section][key])

            elif (section == 'TXSPDOCom'):
                for key in sections_of_ini_file[section]:
                    self.assertTrue(hasattr(settings.spdoCfg.tx, key))
                    self.assertEqual(getattr(settings.spdoCfg.tx, key),
                                     sections_of_ini_file[section][key])

            elif (section == 'RXSPDOCom'):
                for key in sections_of_ini_file[section]:
                    self.assertTrue(hasattr(settings.spdoCfg.rx, key))
                    self.assertEqual(getattr(settings.spdoCfg.rx, key),
                                     sections_of_ini_file[section][key])

            elif (section == 'SCMCfg'):
                for key in sections_of_ini_file[section]:
                    self.assertTrue(hasattr(settings.scmCfg, key))
                    self.assertEqual(getattr(settings.scmCfg, key),
                                     sections_of_ini_file[section][key])

    ##
    # \brief Tests reading of a config / settings file with a missing first
    # header
    def test_read_configfile_missing_header(self):
        with open(os.path.join(Test_configparser.testdata_path,
                               'settings_missing_sectionheader.ini'), 'r') as f:
            with self.assertRaises(SystemExit):
                self.configparser.read_configfile(f)

    ##
    # \brief Tests reading of a cofing / settings file with missing assignments
    def test_read_configfile_no_assignment(self):
        with open(os.path.join(Test_configparser.testdata_path,
                               'settings_no_assignment.ini'), 'r') as f:
            with self.assertRaises(SystemExit):
                self.configparser.read_configfile(f)

    ##
    # \brief Tests reading of a cofing / settings file with double entries in
    # the same section. This is only recognized by configparser from Pyton
    # version 3.2 on
    @unittest.skipIf(current_python_version < python32,
                     'Duplicate options are recognized by ConfigParser from \
                     Python v3.2 on')
    def test_read_configfile_double_entries(self):
        with open(os.path.join(Test_configparser.testdata_path,
                               'settings_double_entries.ini'), 'r') as f:
            with self.assertRaises(SystemExit):
                self.configparser.read_configfile(f)

    ##
    # \brief Tests reading of a cofing / settings file with missing sections
    def test_get_settings_missing_sections(self):
        with open(os.path.join(Test_configparser.testdata_path,
                               'settings_missing_sections.ini'), 'r') as f:
            self.configparser.read_configfile(f)

            with self.assertRaises(SystemExit):
                self.configparser.get_settings()

    ##
    # \brief Tests reading of a config / settings file with wrong attribute
    # names
    def test_get_settings_wrong_attributenames(self):
        with open(os.path.join(Test_configparser.testdata_path,
                               'settings_wrong_attributenames.ini'), 'r') as f:
            self.configparser.read_configfile(f)

            with self.assertRaises(SystemExit):
                self.configparser.get_settings()

    ##
    # \brief Tests the config printing method
    def test_print_config(self):
        settings_file_path = os.path.join(Test_configparser.testdata_path,
                                          'pysodbsettings.ini')

        with open(settings_file_path, 'r') as f:
            self.configparser.read_configfile(f)
        self.configparser.print_config()

if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
