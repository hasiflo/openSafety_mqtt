##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file geneplscfgh.py
# Functionality to generate EPLScfg.h
#
# This file acts as interface for the code code generation and encapsulates
# all functionality directly needed to generate parts of the file EPLScfg.h.

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

# third party packages or modules

# own packages or modules
from pysodb.codegeneration.codegenutil import generatorfunc, CodeGenUtil
from pysodb.codegeneration.common import getepls_enable_disable_from_string, \
Constants, DataChooser, FromOSDDDataStore, FromSettingsDataStore


##
# \brief This class links the input source files with the code generation logic
#
# The GenEPLScfgh class acts as link between the input source files with their
# specific markup and the logic for generation the desired code.
# It encapsulates all functions, which can be called from outside i.e.
# the input source file.
class GenEPLScfgh(CodeGenUtil):

    ##
    # \brief Initialisation function
    # \param codegenutildata Instance of CodeGenUtilData
    # \param language Language of the code which should get generated
    def __init__(self, codegenutildata, language):
        super(GenEPLScfgh, self).__init__(codegenutildata, language)

        self.osddds = FromOSDDDataStore(self.osdd_module)
        self.settingsds = FromSettingsDataStore(self.sodb_settings)
        self.datachooser = DataChooser(
            codegenutildata.overrule_osdd, self.settingsds, self.osddds)

    ##
    # \brief Generates the define EPLS_k_CONFIGURATION_STRING for the file
    # EPLSCfg.h
    @generatorfunc
    def EPLS_k_CONFIGURATION_STRING(self):
        if len(self.settingsds.epls_k_configuration_string) > 0:
            value = self.settingsds.epls_k_configuration_string
        else:
            value = self.osddds.epls_k_configuration_string

        if not value.startswith('"'):
            value = '"' + value
        if not value.endswith('"'):
            value = value + '"'
        self.codegen.gen_definition('EPLS_k_CONFIGURATION_STRING', value)

    ##
    # \brief Generates the define EPLS_cfg_MAX_INSTANCES for the file EPLSCfg.h
    @generatorfunc
    def EPLS_cfg_MAX_INSTANCES(self):
        value = int(self.sodb_settings.maxInstances)
        self.codegen.gen_definition('EPLS_cfg_MAX_INSTANCES', value)

    ##
    # \brief Generates the define EPLS_cfg_ERROR_STRING for the file EPLSCfg.h
    @generatorfunc
    def EPLS_cfg_ERROR_STRING(self):
        value = getepls_enable_disable_from_string(
            self.sodb_settings.errorString)
        self.codegen.gen_definition('EPLS_cfg_ERROR_STRING', value)

    ##
    # \brief Generates the define EPLS_cfg_ERROR_STATISTIC for the file
    # EPLSCfg.h
    @generatorfunc
    def EPLS_cfg_ERROR_STATISTIC(self):
        value = getepls_enable_disable_from_string(
            self.sodb_settings.errorStatistic)
        self.codegen.gen_definition('EPLS_cfg_ERROR_STATISTIC', value)

    ##
    # \brief Generates the define EPLS_cfg_MAX_PYLD_LEN for the file EPLSCfg.h
    @generatorfunc
    def EPLS_cfg_MAX_PYLD_LEN(self):
        value = self.datachooser.get('epls_cfg_max_pyld_len')

        if value < Constants.SSDO_MIN_PAYLOAD_LENGTH:
            value = Constants.SSDO_MIN_PAYLOAD_LENGTH
        self.codegen.gen_definition('EPLS_cfg_MAX_PYLD_LEN', value)

    ##
    # \brief Generates the define EPLS_cfg_SAPL_REPORT_STATE_CHANGE for the
    # file EPLSCfg.h
    @generatorfunc
    def EPLS_cfg_SAPL_REPORT_STATE_CHANGE(self):
        value = getepls_enable_disable_from_string(
            self.sodb_settings.saplReportStateChange)
        self.codegen.gen_definition('EPLS_cfg_SAPL_REPORT_STATE_CHANGE', value)

    ##
    # \brief Generates the define SPDO_cfg_PROP_DELAY_STATISTIC for the file
    # EPLSCfg.h
    @generatorfunc
    def SPDO_cfg_PROP_DELAY_STATISTIC(self):
        value = getepls_enable_disable_from_string(
            self.sodb_settings.spdoCfg.propDelayStatistic)
        self.codegen.gen_definition('SPDO_cfg_PROP_DELAY_STATISTIC', value)

    ##
    # \brief Generates the define SPDO_cfg_40_BIT_CT_SUPPORT  for the file
    # EPLSCfg.h
    @generatorfunc
    def SPDO_cfg_40_BIT_CT_SUPPORT(self):
        value = getepls_enable_disable_from_string(
            self.sodb_settings.spdoCfg.spdo40bitCTSupport)
        self.codegen.gen_definition('SPDO_cfg_40_BIT_CT_SUPPORT', value)

    ##
    # \brief Generates the define SPDO_cfg_EXTENDED_CT_BIT_FIELD  for the file
    # EPLSCfg.h
    @generatorfunc
    def SPDO_cfg_EXTENDED_CT_BIT_FIELD(self):
        value = getepls_enable_disable_from_string(
            self.sodb_settings.spdoCfg.spdoExtendedCTBitField)
        self.codegen.gen_definition('SPDO_cfg_EXTENDED_CT_BIT_FIELD', value)

    ##
    # \brief Generates the define SPDO_cfg_FRAME_CPY_INTERN for the file
    # EPLSCfg.h
    @generatorfunc
    def SPDO_cfg_FRAME_CPY_INTERN(self):
        value = getepls_enable_disable_from_string(
            self.sodb_settings.spdoCfg.frameCopyIntern)
        self.codegen.gen_definition('SPDO_cfg_FRAME_CPY_INTERN', value)

    ##
    # \brief Generates the define SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE for the file
    # EPLSCfg.h
    @generatorfunc
    def SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE(self):
        value = getepls_enable_disable_from_string(
            self.sodb_settings.spdoCfg.numLookupTable)
        self.codegen.gen_definition('SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE', value)

    ##
    # \brief Generates the define SPDO_cfg_MAX_SYNC_RX_SPDO for the file
    # EPLSCfg.h
    @generatorfunc
    def SPDO_cfg_MAX_SYNC_RX_SPDO(self):
        value = int(self.sodb_settings.spdoCfg.maxRxSyncedPerTx)
        self.codegen.gen_definition('SPDO_cfg_MAX_SYNC_RX_SPDO', value)

    ##
    # \brief Generates the define SPDO_cfg_CONNECTION_VALID_BIT_FIELD for the
    # file EPLSCfg.h
    @generatorfunc
    def SPDO_cfg_CONNECTION_VALID_BIT_FIELD(self):
        value = getepls_enable_disable_from_string(
            self.sodb_settings.spdoCfg.connectionValidBitField)
        self.codegen.gen_definition(
            'SPDO_cfg_CONNECTION_VALID_BIT_FIELD', value)

    ##
    # \brief Generates the define SPDO_cfg_CONNECTION_VALID_STATISTIC for the
    # file EPLSCfg.h
    @generatorfunc
    def SPDO_cfg_CONNECTION_VALID_STATISTIC(self):
        value = getepls_enable_disable_from_string(
            self.sodb_settings.spdoCfg.connectionValidStatistic)
        self.codegen.gen_definition(
            'SPDO_cfg_CONNECTION_VALID_STATISTIC', value)

    ##
    # \brief Generates the define SPDO_cfg_MAX_NO_TX_SPDO for the file EPLSCfg.h
    @generatorfunc
    def SPDO_cfg_MAX_NO_TX_SPDO(self):
        value = self.datachooser.get('spdo_cfg_max_no_tx_spdo')
        self.codegen.gen_definition('SPDO_cfg_MAX_NO_TX_SPDO', value)

    ##
    # \brief Generates the define SPDO_cfg_NO_TX_SPDO_ACT_PER_CALL for the file
    # EPLSCfg.h
    @generatorfunc
    def SPDO_cfg_NO_TX_SPDO_ACT_PER_CALL(self):
        value = int(self.sodb_settings.spdoCfg.tx.spdosActivatedPerCall)
        max_no_tx_spdo = self.datachooser.get('spdo_cfg_max_no_tx_spdo')
        # ensure, that value is maximum MAX_NO_TX_SPDO
        if max_no_tx_spdo < value:
            value = max_no_tx_spdo
        self.codegen.gen_definition('SPDO_cfg_NO_TX_SPDO_ACT_PER_CALL', value)

    ##
    # \brief Generates the define SPDO_cfg_MAX_TX_SPDO_MAPP_ENTRIES for the
    # file EPLSCfg.h
    @generatorfunc
    def SPDO_cfg_MAX_TX_SPDO_MAPP_ENTRIES(self):
        value = self.datachooser.get('spdo_cfg_max_tx_spdo_mapp_entries')
        self.codegen.gen_definition('SPDO_cfg_MAX_TX_SPDO_MAPP_ENTRIES', value)

    ##
    # \brief Generates the define SPDO_cfg_MAX_LEN_OF_TX_SPDO for the file
    # EPLSCfg.h
    @generatorfunc
    def SPDO_cfg_MAX_LEN_OF_TX_SPDO(self):
        # using the maximum octet size of the
        # DataTransports with direction 'Input'
        value = self.datachooser.get('spdo_cfg_max_len_of_tx_spdo')
        self.codegen.gen_definition('SPDO_cfg_MAX_LEN_OF_TX_SPDO', value)

    ##
    # \brief Generates the define SPDO_cfg_MAX_NO_RX_SPDO for the file EPLSCfg.h
    @generatorfunc
    def SPDO_cfg_MAX_NO_RX_SPDO(self):
        value = self.datachooser.get('spdo_cfg_max_no_rx_spdo')
        self.codegen.gen_definition('SPDO_cfg_MAX_NO_RX_SPDO', value)

    ##
    # \brief Generates the define SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL for the file
    # EPLSCfg.h
    @generatorfunc
    def SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL(self):
        max_no_rx_spdo = self.datachooser.get('spdo_cfg_max_no_rx_spdo')
        no_rx_spdo_act_per_call = int(
            self.sodb_settings.spdoCfg.rx.spdosActivatedPerCall)
        if max_no_rx_spdo == 0:
            value = Constants.EPLS_k_NOT_APPLICABLE
        else:
            if max_no_rx_spdo < no_rx_spdo_act_per_call:
                value = max_no_rx_spdo
            else:
                value = no_rx_spdo_act_per_call

        self.codegen.gen_definition('SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL', value)

    ##
    # \brief Generates the define SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES for the
    # file EPLSCfg.h
    @generatorfunc
    def SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES(self):
        if self.datachooser.get('spdo_cfg_max_no_rx_spdo') == 0:
            value = Constants.EPLS_k_NOT_APPLICABLE
        else:
            value = self.datachooser.get('spdo_cfg_max_rx_spdo_mapp_entries')
        self.codegen.gen_definition('SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES', value)

    ##
    # \brief Generates the define SPDO_cfg_MAX_LEN_OF_RX_SPDO for the file
    # EPLSCfg.h
    @generatorfunc
    def SPDO_cfg_MAX_LEN_OF_RX_SPDO(self):
        # using the maximum octet size of the
        # DataTransports with direction 'Output'
        if self.datachooser.get('spdo_cfg_max_no_rx_spdo') == 0:
            value = Constants.EPLS_k_NOT_APPLICABLE
        else:
            value = self.datachooser.get('spdo_cfg_max_len_of_rx_spdo')
        self.codegen.gen_definition('SPDO_cfg_MAX_LEN_OF_RX_SPDO', value)

    ##
    # \brief Generates the define SPDO_cfg_NO_NOT_ANSWERED_TR for the file
    # EPLSCfg.h
    @generatorfunc
    def SPDO_cfg_NO_NOT_ANSWERED_TR(self):
        if self.datachooser.get('spdo_cfg_max_no_rx_spdo') == 0:
            value = Constants.EPLS_k_NOT_APPLICABLE
        else:
            value = int(self.sodb_settings.spdoCfg.notAnsweredTr)
        self.codegen.gen_definition('SPDO_cfg_NO_NOT_ANSWERED_TR', value)

    ##
    # \brief Generates the define SOD_cfg_APPLICATION_OBJ for the file EPLSCfg.h
    @generatorfunc
    def SOD_cfg_APPLICATION_OBJ(self):
        value = getepls_enable_disable_from_string(
            self.sodb_settings.applicationObjects)
        self.codegen.gen_definition('SOD_cfg_APPLICATION_OBJ', value)


##
# \brief Intialisation function of the module.
#
# When this module is used by an input source file,
# this function creates an instance of the needed CodeGenUtil sublcass
# which then can be used for code generation
def init():
    from pysodb.codegeneration.codegenutil import cgendata
    cg = GenEPLScfgh(cgendata, 'C')
    return cg

##
# \}
# \}
