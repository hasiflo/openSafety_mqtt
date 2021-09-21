##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file test_geneplscfgh.py
# Unit test for the module pysodb.codegeneration.geneplscfgh

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
import sys

# third party packages or modules

# own packages or modules
from tests.common import DummyDataStore, DummyDataChooser, \
    DummyCodeGenUtilData, dummy_getepls_enable_disable_from_string, \
    dummy_generatorfunc_decorator, OutputCatcher, DummyCodeGen, print_wrapper

import pysodb.codegeneration.common
import pysodb.codegeneration.codegenutil

# patching the function decorator, this must be done before anything of the
# package or module is imported for the first time
orig_generatorfunc_decorator = pysodb.codegeneration.codegenutil.generatorfunc
pysodb.codegeneration.codegenutil.generatorfunc = dummy_generatorfunc_decorator

import pysodb.codegeneration.geneplscfgh

pysodb.codegeneration.codegenutil.generatorfunc = orig_generatorfunc_decorator

def tearDownModule():
    sys.modules.pop('pysodb.codegeneration.geneplscfgh')

##
# \brief TestCase base class, unifies common code for deriving specific
# TestCase classes
class Test_geneplscfgh_Base(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        super(Test_geneplscfgh_Base, cls).setUpClass()
        # patching out used classes and functions
        cls.orig_getepls_enable_disable_from_string = \
            pysodb.codegeneration.common.getepls_enable_disable_from_string
        cls.orig_FromSettingsDataStore = \
            pysodb.codegeneration.common.FromSettingsDataStore
        cls.orig_FromOSDDDataStore = \
            pysodb.codegeneration.common.FromOSDDDataStore
        cls.orig_DataChooser = pysodb.codegeneration.common.DataChooser

        pysodb.codegeneration.geneplscfgh.getepls_enable_disable_from_string = \
            dummy_getepls_enable_disable_from_string
        pysodb.codegeneration.geneplscfgh.FromOSDDDataStore = DummyDataStore
        pysodb.codegeneration.geneplscfgh.FromSettingsDataStore = DummyDataStore
        pysodb.codegeneration.geneplscfgh.DataChooser = DummyDataChooser

    @classmethod
    def tearDownClass(cls):
        super(Test_geneplscfgh_Base, cls).tearDownClass()
        pysodb.codegeneration.geneplscfgh.getepls_enable_disable_from_string = \
            pysodb.codegeneration.common.getepls_enable_disable_from_string
        pysodb.codegeneration.geneplscfgh.FromSettingsDataStore = \
            cls.orig_FromSettingsDataStore
        pysodb.codegeneration.geneplscfgh.FromOSDDDataStore = \
            cls.orig_FromOSDDDataStore
        pysodb.codegeneration.geneplscfgh.DataChooser = cls.orig_DataChooser
        pysodb.codegeneration.codegenutil.generatorfunc = \
            orig_generatorfunc_decorator


##
# \brief Tests GenEPLScfgh class initialisation
class Test_geneplscfgh(Test_geneplscfgh_Base):

    @classmethod
    def setUpClass(cls):
        super(Test_geneplscfgh, cls).setUpClass()

    @classmethod
    def tearDownClass(cls):
        super(Test_geneplscfgh, cls).tearDownClass()

    ##
    # \brief Tests init() function of geneplscfgh
    def test_init(self):
        cud = DummyCodeGenUtilData()
        pysodb.codegeneration.codegenutil.cgendata = cud

        from pysodb.codegeneration.geneplscfgh import init, GenEPLScfgh

        retval = init()
        self.assertTrue(isinstance(retval, GenEPLScfgh))
        self.assertEqual(retval.language, 'C')
        self.assertTrue(cud is retval.codegenutildata)

    ##
    # \brief Tests GenEPLScfgh class initialisation
    def test_GenEPLScfgh_init(self):

        cud = DummyCodeGenUtilData()
        pysodb.codegeneration.codegenutil.cgendata = cud

        from pysodb.codegeneration.geneplscfgh import GenEPLScfgh

        genobj = GenEPLScfgh(cud, 'C')
        self.assertTrue(isinstance(genobj.osddds, DummyDataStore))
        self.assertTrue(isinstance(genobj.settingsds, DummyDataStore))
        self.assertTrue(isinstance(genobj.datachooser, DummyDataChooser))


##
# \brief Tests GenEPLScfgh class methods, which generate specific code / defines
# for the EPLSCfg.h file
class Test_geneplscfgh_class_methods(Test_geneplscfgh_Base):

    @classmethod
    def setUpClass(cls):
        super(Test_geneplscfgh_class_methods, cls).setUpClass()

    @classmethod
    def tearDownClass(cls):
        super(Test_geneplscfgh_class_methods, cls).tearDownClass()

    def setUp(self):
        Test_geneplscfgh_Base.setUp(self)
        cud = DummyCodeGenUtilData()
        cud.outfuncs = [print_wrapper, OutputCatcher.out]
        from pysodb.codegeneration.geneplscfgh import GenEPLScfgh
        self.genobj = GenEPLScfgh(cud, 'C')
        self.genobj.codegen = DummyCodeGen()

    ##
    # \brief Tests generation of EPLS_k_CONFIGURATION_STRING define
    def test_EPLS_k_CONFIGURATION_STRING(self):
        self.genobj.osddds.epls_k_configuration_string = \
            'test_osddds_configstring'
        self.genobj.settingsds.epls_k_configuration_string = ''
        self.genobj.EPLS_k_CONFIGURATION_STRING()

        self.genobj.settingsds.epls_k_configuration_string = '""'
        self.genobj.EPLS_k_CONFIGURATION_STRING()

        self.genobj.settingsds.epls_k_configuration_string = \
            'test_settingds_configstring'
        self.genobj.EPLS_k_CONFIGURATION_STRING()

        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 3)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('EPLS_k_CONFIGURATION_STRING', '"test_osddds_configstring"'),
              {}),
             (('EPLS_k_CONFIGURATION_STRING', '""'),
              {}),
             (('EPLS_k_CONFIGURATION_STRING', '"test_settingds_configstring"'),
              {})])

    ##
    # \brief Tests generation of EPLS_cfg_MAX_INSTANCES define
    def test_EPLS_cfg_MAX_INSTANCES(self):
        self.genobj.sodb_settings.maxInstances = 10

        self.genobj.EPLS_cfg_MAX_INSTANCES()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('EPLS_cfg_MAX_INSTANCES', 10), {})])

        self.genobj.sodb_settings.maxInstances = 'expect_error'

        with self.assertRaises(ValueError):
            self.genobj.EPLS_cfg_MAX_INSTANCES()

    ##
    # \brief Tests generation of EPLS_cfg_ERROR_STRING define
    def test_EPLS_cfg_ERROR_STRING(self):
        self.genobj.sodb_settings.errorString = 'test_errorstring'

        self.genobj.EPLS_cfg_ERROR_STRING()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('EPLS_cfg_ERROR_STRING', 'test_errorstring'), {})])

    ##
    # \brief Tests generation of EPLS_cfg_ERROR_STATISTIC define
    def test_EPLS_cfg_ERROR_STATISTIC(self):
        self.genobj.sodb_settings.errorStatistic = 'test_errorstatistic'

        self.genobj.EPLS_cfg_ERROR_STATISTIC()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('EPLS_cfg_ERROR_STATISTIC', 'test_errorstatistic'), {})])

    ##
    # \brief Tests generation of EPLS_cfg_MAX_PYLD_LEN define
    def test_EPLS_cfg_MAX_PYLD_LEN(self):
        self.genobj.datachooser.returnvalue = 20

        self.genobj.EPLS_cfg_MAX_PYLD_LEN()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('EPLS_cfg_MAX_PYLD_LEN', 20), {})])

        self.genobj.datachooser.returnvalue = 0
        self.genobj.codegen.reset()

        self.genobj.EPLS_cfg_MAX_PYLD_LEN()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)

        # check for expected payload length
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('EPLS_cfg_MAX_PYLD_LEN', 8), {})])

    ##
    # \brief Tests generation of test_EPLS_cfg_SAPL_REPORT_STATE_CHANGE define
    def test_EPLS_cfg_SAPL_REPORT_STATE_CHANGE(self):
        self.genobj.sodb_settings.saplReportStateChange = \
            'test_saplReportStateChange'

        self.genobj.EPLS_cfg_SAPL_REPORT_STATE_CHANGE()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('EPLS_cfg_SAPL_REPORT_STATE_CHANGE',
               'test_saplReportStateChange'), {})])

    ##
    # \brief Tests generation of EPLS_cfg_ERROR_STATISTIC define
    def test_SPDO_cfg_PROP_DELAY_STATISTIC(self):
        self.genobj.sodb_settings.spdoCfg.propDelayStatistic = \
            'test_propDelayStatistic'

        self.genobj.SPDO_cfg_PROP_DELAY_STATISTIC()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_PROP_DELAY_STATISTIC',
               'test_propDelayStatistic'), {})])

    ##
    # \brief Tests generation of SPDO_cfg_40_BIT_CT_SUPPORT define
    def test_SPDO_cfg_40_BIT_CT_SUPPORT(self):
        self.genobj.sodb_settings.spdoCfg.spdo40bitCTSupport = \
            'test_spdo40bitCTSupport'

        self.genobj.SPDO_cfg_40_BIT_CT_SUPPORT()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_40_BIT_CT_SUPPORT',
               'test_spdo40bitCTSupport'), {})])

    # \brief Tests generation of SPDO_cfg_EXTENDED_CT_BIT_FIELD define
    def test_SPDO_cfg_EXTENDED_CT_BIT_FIELD(self):
        self.genobj.sodb_settings.spdoCfg.spdoExtendedCTBitField = \
            'test_spdoExtendedCTBitField'

        self.genobj.SPDO_cfg_EXTENDED_CT_BIT_FIELD()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_EXTENDED_CT_BIT_FIELD',
               'test_spdoExtendedCTBitField'), {})])

    ##
    # \brief Tests generation of SPDO_cfg_FRAME_CPY_INTERN define
    def test_SPDO_cfg_FRAME_CPY_INTERN(self):
        self.genobj.sodb_settings.spdoCfg.frameCopyIntern = 'test_framecpy'

        self.genobj.SPDO_cfg_FRAME_CPY_INTERN()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_FRAME_CPY_INTERN', 'test_framecpy'), {})])

    ##
    # \brief Tests generation of SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE define
    def test_SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE(self):
        self.genobj.sodb_settings.spdoCfg.numLookupTable = \
        'test_spdonumlookuptable'

        self.genobj.SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE', 'test_spdonumlookuptable'),
              {})])

    ##
    # \brief Tests generation of SPDO_cfg_MAX_SYNC_RX_SPDO define
    def test_SPDO_cfg_MAX_SYNC_RX_SPDO(self):
        self.genobj.sodb_settings.spdoCfg.maxRxSyncedPerTx = 30

        self.genobj.SPDO_cfg_MAX_SYNC_RX_SPDO()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_MAX_SYNC_RX_SPDO', 30), {})])

        self.genobj.sodb_settings.spdoCfg.maxRxSyncedPerTx = \
            'test_maxrxsyncedpertx'
        with self.assertRaises(ValueError):
            self.genobj.SPDO_cfg_MAX_SYNC_RX_SPDO()

    ##
    # \brief Tests generation of SPDO_cfg_CONNECTION_VALID_BIT_FIELD define
    def test_SPDO_cfg_CONNECTION_VALID_BIT_FIELD(self):
        self.genobj.sodb_settings.spdoCfg.connectionValidBitField = \
            'test_connectionvalidbitfield'

        self.genobj.SPDO_cfg_CONNECTION_VALID_BIT_FIELD()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_CONNECTION_VALID_BIT_FIELD',
               'test_connectionvalidbitfield'), {})])

    ##
    # \brief Tests generation of SPDO_cfg_CONNECTION_VALID_STATISTIC define
    def test_SPDO_cfg_CONNECTION_VALID_STATISTIC(self):
        self.genobj.sodb_settings.spdoCfg.connectionValidStatistic = \
            'test_connectionvalidstatistic'

        self.genobj.SPDO_cfg_CONNECTION_VALID_STATISTIC()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_CONNECTION_VALID_STATISTIC',
               'test_connectionvalidstatistic'), {})])

    ##
    # \brief Tests generation of SPDO_cfg_MAX_NO_TX_SPDO define
    def test_SPDO_cfg_MAX_NO_TX_SPDO(self):
        self.genobj.datachooser.returnvalue = 40

        self.genobj.SPDO_cfg_MAX_NO_TX_SPDO()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_MAX_NO_TX_SPDO', 40), {})])

        self.genobj.datachooser.returnvalue = 'test_spdocfgmaxnotxspdo'

        self.genobj.codegen.reset()

        self.genobj.SPDO_cfg_MAX_NO_TX_SPDO()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_MAX_NO_TX_SPDO', 'test_spdocfgmaxnotxspdo'), {})])

    ##
    # \brief Tests generation of SPDO_cfg_NO_TX_SPDO_ACT_PER_CALL define
    def test_SPDO_cfg_NO_TX_SPDO_ACT_PER_CALL(self):
        self.genobj.sodb_settings.spdoCfg.tx.spdosActivatedPerCall = 50
        self.genobj.datachooser.returnvalue = 60

        self.genobj.SPDO_cfg_NO_TX_SPDO_ACT_PER_CALL()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_NO_TX_SPDO_ACT_PER_CALL', 50), {})])

        self.genobj.sodb_settings.spdoCfg.tx.spdosActivatedPerCall = 66
        self.genobj.datachooser.returnvalue = 55

        self.genobj.codegen.reset()

        self.genobj.SPDO_cfg_NO_TX_SPDO_ACT_PER_CALL()

        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_NO_TX_SPDO_ACT_PER_CALL', 55), {})])

        self.genobj.sodb_settings.spdoCfg.tx.spdosActivatedPerCall = \
            'test_notxspdoactpercall'
        self.genobj.datachooser.returnvalue = 55

        with self.assertRaises(ValueError):
            self.genobj.SPDO_cfg_NO_TX_SPDO_ACT_PER_CALL()

    ##
    # \brief Tests generation of SPDO_cfg_MAX_TX_SPDO_MAPP_ENTRIES define
    def test_SPDO_cfg_MAX_TX_SPDO_MAPP_ENTRIES(self):
        self.genobj.datachooser.returnvalue = 70

        self.genobj.SPDO_cfg_MAX_TX_SPDO_MAPP_ENTRIES()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_MAX_TX_SPDO_MAPP_ENTRIES', 70), {})])

    ##
    # \brief Tests generation of SPDO_cfg_MAX_LEN_OF_TX_SPDO define
    def test_SPDO_cfg_MAX_LEN_OF_TX_SPDO(self):
        self.genobj.datachooser.returnvalue = 80

        self.genobj.SPDO_cfg_MAX_LEN_OF_TX_SPDO()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_MAX_LEN_OF_TX_SPDO', 80), {})])

    ##
    # \brief Tests generation of SPDO_cfg_MAX_NO_RX_SPDO define
    def test_SPDO_cfg_MAX_NO_RX_SPDO(self):
        self.genobj.datachooser.returnvalue = 90

        self.genobj.SPDO_cfg_MAX_NO_RX_SPDO()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_MAX_NO_RX_SPDO', 90), {})])

    ##
    # \brief Tests generation of SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL define
    def test_SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL(self):
        self.genobj.datachooser.returnvalue = 100
        self.genobj.sodb_settings.spdoCfg.rx.spdosActivatedPerCall = 101

        self.genobj.SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL', 100), {})])

        self.genobj.datachooser.returnvalue = 103
        self.genobj.sodb_settings.spdoCfg.rx.spdosActivatedPerCall = 102

        self.genobj.codegen.reset()

        self.genobj.SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL', 102), {})])

        self.genobj.datachooser.returnvalue = 0
        self.genobj.sodb_settings.spdoCfg.rx.spdosActivatedPerCall = 104

        self.genobj.codegen.reset()

        self.genobj.SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL',
               'EPLS_k_NOT_APPLICABLE'), {})])

        self.genobj.datachooser.returnvalue = 105
        self.genobj.sodb_settings.spdoCfg.rx.spdosActivatedPerCall = \
            'test_spdocfgnorxspdoactpercall'

        with self.assertRaises(ValueError):
            self.genobj.SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL()

    ##
    # \brief Tests generation of SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES define
    def test_SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES(self):

        self.genobj.datachooser.returnvalue = 0

        self.genobj.SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES',
               'EPLS_k_NOT_APPLICABLE'), {})])

        self.genobj.datachooser.returnvalue = 110

        self.genobj.codegen.reset()

        self.genobj.SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES', 110), {})])

    ##
    # \brief Tests generation of SPDO_cfg_MAX_LEN_OF_RX_SPDO define
    def test_SPDO_cfg_MAX_LEN_OF_RX_SPDO(self):

        self.genobj.datachooser.returnvalue = 0

        self.genobj.SPDO_cfg_MAX_LEN_OF_RX_SPDO()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_MAX_LEN_OF_RX_SPDO', 'EPLS_k_NOT_APPLICABLE'), {})])

        self.genobj.datachooser.returnvalue = 110
        self.genobj.codegen.reset()

        self.genobj.SPDO_cfg_MAX_LEN_OF_RX_SPDO()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_MAX_LEN_OF_RX_SPDO', 110), {})])

    ##
    # \brief Tests generation of SPDO_cfg_NO_NOT_ANSWERED_TR define
    def test_SPDO_cfg_NO_NOT_ANSWERED_TR(self):

        self.genobj.datachooser.returnvalue = 0

        self.genobj.SPDO_cfg_NO_NOT_ANSWERED_TR()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_NO_NOT_ANSWERED_TR', 'EPLS_k_NOT_APPLICABLE'), {})])

        self.genobj.datachooser.returnvalue = 120
        self.genobj.sodb_settings.spdoCfg.notAnsweredTr = 125

        self.genobj.codegen.reset()

        self.genobj.SPDO_cfg_NO_NOT_ANSWERED_TR()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SPDO_cfg_NO_NOT_ANSWERED_TR', 125), {})])

    ##
    # \brief Tests generation of SOD_cfg_APPLICATION_OBJ define
    def test_SOD_cfg_APPLICATION_OBJ(self):

        self.genobj.sodb_settings.applicationObjects = \
            'test_sodcfgapplicationobjects'

        self.genobj.SOD_cfg_APPLICATION_OBJ()
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_number_of_calls(), 1)
        self.assertEqual(
            self.genobj.codegen.gen_definition.get_function_call_params(),
            [(('SOD_cfg_APPLICATION_OBJ',
               'test_sodcfgapplicationobjects'), {})])

if __name__ == '__main__':
    unittest.main()

##
# \}
# \}
# \}
