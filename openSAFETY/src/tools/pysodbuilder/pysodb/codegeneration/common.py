##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file codegeneration/common.py
# Common data and functionality used by several generation files (genXYZ.py)
#

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
from pysodb.util.osdd_data import OSDDDirections


##
# \brief Constants used for code generation
class Constants:

    EPLS_k_NOT_APPLICABLE = 'EPLS_k_NOT_APPLICABLE'
    EPLS_k_DISABLE = 'EPLS_k_DISABLE'
    EPLS_k_ENABLE = 'EPLS_k_ENABLE'

    SSDO_MIN_PAYLOAD_LENGTH = 8

    FALSE_STRINGS = ['False', 'OFF']
    TRUE_STRINGS = ['True', 'ON']


##
# \brief Translates True/ON and False/OFF to EPLS_k_DISABLE and EPLS_k_ENABLE
def getepls_enable_disable_from_string(string):
    if (string in Constants.FALSE_STRINGS):
        return Constants.EPLS_k_DISABLE
    elif (string in Constants.TRUE_STRINGS):
        return Constants.EPLS_k_ENABLE


##
# \brief Base class which stores data the settings file and osdd file have in
# common
#
# This class acts as base class that stores data, which the settings file
# and the osdd file have in common. The \_\_init()\_\_ functions in the derived
# classes provide functionality to populate this data with information
# coming from different (organized) data sources.
class DataStore(object):

    def __init__(self):
        self.epls_cfg_max_pyld_len = 0
        self.epls_k_configuration_string = ''

        self.spdo_cfg_max_no_tx_spdo = 0
        self.spdo_cfg_max_tx_spdo_mapp_entries = 0
        self.spdo_cfg_max_len_of_tx_spdo = 0

        self.spdo_cfg_max_no_rx_spdo = 0
        self.spdo_cfg_max_rx_spdo_mapp_entries = 0
        self.spdo_cfg_max_len_of_rx_spdo = 0


##
# \brief Class which holds data from settings file
class FromSettingsDataStore(DataStore):

    ##
    # \brief Initialisation function
    #
    # \param sodbsettings Object with the data from the settings file
    # (see \ref core.common.EPLSCfg) which should be loaded / linked to the
    # new data structure
    def __init__(self, sodbsettings):
        super(FromSettingsDataStore, self).__init__()
        self.epls_cfg_max_pyld_len = int(sodbsettings.maxPayloadLength)
        self.epls_k_configuration_string = sodbsettings.constants.configString

        self.spdo_cfg_max_no_tx_spdo = int(sodbsettings.spdoCfg.tx.maxSpdo)
        self.spdo_cfg_max_tx_spdo_mapp_entries = int(
            sodbsettings.spdoCfg.tx.maxMapEntries)
        self.spdo_cfg_max_len_of_tx_spdo = int(
            sodbsettings.spdoCfg.tx.maxPayloadSize)

        self.spdo_cfg_max_no_rx_spdo = int(sodbsettings.spdoCfg.rx.maxSpdo)
        self.spdo_cfg_max_rx_spdo_mapp_entries = int(
            sodbsettings.spdoCfg.rx.maxMapEntries)
        self.spdo_cfg_max_len_of_rx_spdo = int(
            sodbsettings.spdoCfg.rx.maxPayloadSize)


##
# \brief Class which holds data from osdd file
class FromOSDDDataStore(DataStore):

    ##
    # \brief Initialisation function
    #
    # \param osdd_module Object with the data from the osdd file
    # which should be loaded / linked to the new data structure
    def __init__(self, osdd_module):
        super(FromOSDDDataStore, self).__init__()

        self._acyclic_transport = \
            osdd_module.ModuleDefinition.DataTransport.ACyclicTransport
        self._spdotransports = \
            osdd_module.ChannelConfiguration.DataTransport.SPDOTransport

        self._max_octetsize_tx_spdo = 0
        self._max_octetsize_rx_spdo = 0
        self._maxnr_input_mappings = 0
        self._maxnr_output_mappings = 0
        self._spdotransports_input = []
        self._spdotransports_output = []

        self._process_spdo_data()

        self.epls_cfg_max_pyld_len = self._get_max_acyclic_payload()
        self.epls_k_configuration_string = osdd_module.id

        self.spdo_cfg_max_no_tx_spdo = self._get_maxnr_tx_spdo()
        self.spdo_cfg_max_tx_spdo_mapp_entries = self._get_maxnr_tx_mappings()
        self.spdo_cfg_max_len_of_tx_spdo = self._get_max_len_of_tx_spdo()

        self.spdo_cfg_max_no_rx_spdo = self._get_maxnr_rx_spdo()
        self.spdo_cfg_max_rx_spdo_mapp_entries = self._get_maxnr_rx_mappings()
        self.spdo_cfg_max_len_of_rx_spdo = self._get_max_len_of_rx_spdo()

    ##
    # \brief Calculates maximum acyclic payload in bytes
    #
    # The maximum acyclic payload in bytes is calculated out of the
    # OctetSize attribute from an AcyclicTransport entity from the osdd file
    def _get_max_acyclic_payload(self):
        max_ssdo_payload_octet_len = 0
        for item in self._acyclic_transport:
            if max_ssdo_payload_octet_len < item.OctetSize:
                max_ssdo_payload_octet_len = item.OctetSize
        return int(max_ssdo_payload_octet_len / 8)

    ##
    # \brief Counts number of SPDOs and maximum number of mappings and the
    # maximum OctetSize over all Input / Output Channels from an osdd module
    def _process_spdo_data(self):
        for spdotransport in self._spdotransports:
            if spdotransport.Direction == OSDDDirections.Input:
                self._spdotransports_input.append(spdotransport)
                self._maxnr_input_mappings += int(spdotransport.MaxNrMappings)

                if spdotransport.OctetSize > self._max_octetsize_tx_spdo:
                    self._max_octetsize_tx_spdo = spdotransport.OctetSize

            elif spdotransport.Direction == OSDDDirections.Output:
                self._spdotransports_output.append(spdotransport)
                self._maxnr_output_mappings += int(spdotransport.MaxNrMappings)

                if spdotransport.OctetSize > self._max_octetsize_rx_spdo:
                    self._max_octetsize_rx_spdo = spdotransport.OctetSize

    ##
    # \brief Returns the maximum number of input mappings
    def _get_maxnr_tx_mappings(self):
        return self._maxnr_input_mappings

    ##
    # \brief Returns the maximum number of output mappings
    def _get_maxnr_rx_mappings(self):
        return self._maxnr_output_mappings

    ##
    # \brief Returns the maximum number of tx SPDOs
    def _get_maxnr_tx_spdo(self):
        return len(self._spdotransports_input)

    ##
    # \brief Returns the maximum number of rx SPDOs
    def _get_maxnr_rx_spdo(self):
        return len (self._spdotransports_output)

    ##
    # \brief Returns the maximum size of a tx SPDO in bytes
    def _get_max_len_of_tx_spdo(self):
        return self._get_bytes_from_octetsize(self._max_octetsize_tx_spdo)

    ##
    # \brief Returns the maximum size of a rx SPDO in bytes
    def _get_max_len_of_rx_spdo(self):
        return self._get_bytes_from_octetsize(self._max_octetsize_rx_spdo)

    ##
    # \brief Claculates the payload size in bytes from given octet size
    # This function calculates a SPDO payload size according to the
    # given octet size. Due to different CRC checksum lengths for
    # different payload lengths, the function checks the given octet size
    # and calculates the payload accordingly.
    #
    # \param octetsize OctetSize
    # \return Payload size, if the octet size had a valid value
    def _get_bytes_from_octetsize(self, octetsize):
        modulo = 0
        value = 0

        if octetsize <= 27 and octetsize >= 11:
            value = octetsize - 11
        elif octetsize >= 31:
            value = octetsize - 13
        else:
            raise ValueError('given octet size is not valid')

        modulo = value % 2

        if modulo != 0:
            raise ValueError('given octet size is not valid')

        return int(value / 2)


##
# \brief Class which acts as 'switch' for choosing data between two
# instances of DataStore
#
# The DataChooser class enables the selection of data from two DataStore
# objects based on the overrule_osdd_flag value. If overrule_osdd_flag is true,
# DataChooser returns the value obtained from the settingsfile, if it is bigger
# than the value from the osdd file. Otherwise, it returns the value read from
# the osdd file. If overrule_osdd_flag is false, DataChooser always returns
# the value read from osdd file.
class DataChooser(object):

    ##
    # \brief Initialisation function
    #
    # \param overrule_osdd_flag Flag that influences the choice from where data
    # will be used
    # \param data_from_sodb_settings Object of FromSettingsDataStore
    # \param data_from_osdd Object of FromOSDDDataStore
    def __init__(self, overrule_osdd_flag,
                 data_from_sodb_settings, data_from_osdd):
        self.settingsds = data_from_sodb_settings
        self.osddds = data_from_osdd

        # select data coosing function according to overrule_osdd_flag
        self.func = DataChooser._get_value_from_osdd

        if overrule_osdd_flag:
            self.func = DataChooser._get_value_from_settings_if_bigger

    ##
    # \brief Returns the value chosen by the data coosing function
    #
    # \param attributename Name of the attribute which should be retrieved
    # from one of the DataStore objects
    def get(self, attributename):
        return self.func(self, attributename, self.settingsds, self.osddds)

    ##
    # \brief Returns the value from the settings data store, if the
    # value is bigger than the one read from osdd data store
    #
    # \param attributename Attribute name of the DataStore structures for which
    # the value should be returned
    # \param settingsds Object of FromSettingsDataStore
    # \param osddds Object of FromOSDDDataStore
    def _get_value_from_settings_if_bigger (self, attributename,
                                            settingsds, osddds):
        if hasattr(settingsds, attributename) and \
        hasattr(osddds, attributename):
            settingsval = getattr(settingsds, attributename)
            osddval = getattr(osddds, attributename)

            if settingsval > osddval:
                return settingsval
            else:
                return osddval
        else:
            raise AttributeError (
                'attribute {} not in datastores.'.format(attributename))

    ##
    # \brief Returns always the value from the osdd data store, ignoring the
    # according value of the settings data store
    #
    # \param attributename Attribute name of the DataStore structures for which
    # the value should be returned
    # \param settingsds Object of FromSettingsDataStore
    # \param osddds Object of FromOSDDDataStore
    def _get_value_from_osdd (self, attributename, settingsds, osddds):
        if hasattr(osddds, attributename):
            return getattr(osddds, attributename)
        else:
            raise AttributeError (
                'attribute {} not in datastores.'.format(attributename))

##
# \}
# \}
