##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file osddprocessing/mainprocessing.py
# Implements the main processing logic of an OSDD module.
# OSDD module data will be transformed to a form usable for code generation.
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
from collections import OrderedDict
import re

# third party packages or modules

# own packages or modules
from pysodb.codegeneration.common import getepls_enable_disable_from_string, \
    Constants
from pysodb.osddprocessing.base import OSDDProcessingBase
from pysodb.util.constants import NumberSystemBases, IdentifierPlaceholder, \
    RegExPatterns
from pysodb.util.defaultsod import SODDefaultEntries
from pysodb.util.macro import BitAccessMacro
from pysodb.util.osdd_data import OSDDDirections, OSDDValueTypes
from pysodb.util.sod_data import SODIndexComments, SODAttributes, \
    SODDefaultValue, SODStandardObjects, SODEntryNumEntries, SODAttr, \
    SODEntry, SODCallback, SODDataTypes, SODDefines, SODRange
from pysodb.util.stringfunctions import get_sod_index_string, \
    get_sod_subindex_string, get_identifier_index_string, \
    get_identifier_subindex_string
from pysodb.util.structure import SPDOStruct, Struct, Structmember
from pysodb.util.type_conversion import OSDDValueType_TargetDataType, \
    OSDDValueType_SODDataType


##
# \brief Does the processing of SOD entries and structures for settings
# and SPDO data
class MainProcessing(OSDDProcessingBase):

    ##
    # \brief Starts the main processing logic for OSDD module data
    # transformation
    def do_mainprocessing(self):
        self._generate_spdostructs_and_according_sod_entries()

        self._make_error_register_sod_entry()
        self._make_error_statistics_sod_entries()
        self._make_life_guarding_sod_entries()
        self._make_number_of_retrties_reset_guarding_sod_entry()
        self._make_refresh_interval_of_reset_guarding_sod_entry()
        self._make_device_vendor_information_sod_entries()
        self._make_unique_device_id_sod_entry()
        self._make_parameter_download_sod_entry()
        self._make_common_communication_parameter_sod_entries()

        self._make_mapping_sod_entries()
        self._make_tx_com_sod_entries()
        self._make_rx_com_sod_entries()

        self._generate_settings_structs_paramstream()
        self._generate_settings_sodpos_struct_and_according_sod_entries()
        self._make_vendor_module_specific_sod_entries()
        self._make_end_sod_entry()

    ##
    # \brief Generates structures and SOD entries for SPDO related data
    def _generate_spdostructs_and_according_sod_entries(self):

        for transport in self.processingdata.spdotransport_input \
        + self.processingdata.spdotransport_output:

            structcomment = 'Structure of the SPDOTransport channels of '\
            '{}'.format(transport.name)
            struct = SPDOStruct(transport.name, comment=structcomment)

            if len(transport.channels) > 0:
                self.processingdata.spdo_data_structs.append(struct)

            attrlist = []

            if transport.direction == OSDDDirections.Input:
                comment = SODIndexComments.InputData
                attrlist.append(SODAttributes.RO)
            elif transport.direction == OSDDDirections.Output:
                comment = SODIndexComments.OutputData
                attrlist.append(SODAttributes.RW)

            attrlist.append(SODAttributes.PDO)

            #use ordered dict to maintain order of the appearance of the
            # struct members (channels)
            sod_pos_channels = self._group_channels_to_transports(transport)

            counter_var = 0

            for sod_pos in sod_pos_channels:

                identifier = None
                osddvaluetype = None

                identifier, osddvaluetype, counter_var = \
                    self._get_identifier_datatype_from_channellist \
                        (sod_pos_channels[sod_pos], counter_var)

                #check, if there are Channels with same SOD position but other
                #Transport index

                # it's assumed that if bools are used,
                # an UINT8 will be used, no check of number of bools(> 8!)
                # so the index attribute of a transport element of a channel
                # is not observed, only the lowest byte can be accessed this
                # way
                osddvaluetype = self._change_datatype_if_boolean(osddvaluetype)

                #bitaccess
                self._generate_bit_access_macros(sod_pos_channels[sod_pos],
                                                 struct.identifier, identifier)

                #struct member
                membercomment = ' '.join([transport.name, 'object data',
                                          get_sod_index_string(sod_pos[0]),
                                          get_sod_subindex_string(sod_pos[1])])
                self._make_structmember_translate_datatype_add_to_list(
                    membercomment, struct, identifier, osddvaluetype)

                #create sod entry
                if transport.direction == OSDDDirections.Input:
                    defval = None
                elif transport.direction == OSDDDirections.Output:
                    defval = SODDefaultValue(value='0x00', process_later=True)

                self._make_sodentry_translate_datatype_add_to_list(
                    struct, attrlist[:], defval, sod_pos,
                    identifier, osddvaluetype, self.processingdata.sodentries)

            #create SOD entries for subindexes 0
            self._make_num_entries_sod_entries(
                    comment, sod_pos_channels, self.processingdata.sodentries)

            #refresh struct init
            struct.refresh_initvalues()

    ##
    # \brief Generates the error register SOD entries
    def _make_error_register_sod_entry(self):
        #0x1001 / 0x00
        sodentry = SODDefaultEntries.get_default_sodentries_for_object(
                                        SODStandardObjects.ErrorRegister, 0)
        self.processingdata.sodentries.extend(sodentry)

    ##
    # \brief Generates error statistics SOD entries
    def _make_error_statistics_sod_entries(self):
        #0x1004 / 0x00

        error_statistic = self.processingdata.sodb_settings.errorStatistic
        #only generate error statistics, if this was enabled in settings
        if getepls_enable_disable_from_string(error_statistic) == \
        Constants.EPLS_k_ENABLE:
            sodentries = SODDefaultEntries.get_default_sodentries_for_object(
                                        SODStandardObjects.ErrorStatistics, 0)
            self.processingdata.sodentries.extend(sodentries)

    ##
    # \brief Genereates the life guarding SOD entries
    def _make_life_guarding_sod_entries(self):
        #0x100C / 0x00
        sodentries = SODDefaultEntries.get_default_sodentries_for_object(
                        SODStandardObjects.LifeGuarding, 0)
        self.processingdata.sodentries.extend(sodentries)

    ##
    # \brief Generates the number of retries SOD entry
    def _make_number_of_retrties_reset_guarding_sod_entry(self):
        #0x100D / 0x00
        sodentry = SODDefaultEntries.get_default_sodentries_for_object(
                        SODStandardObjects.NumberOfRetriesResetGuarding, 0)
        self.processingdata.sodentries.extend(sodentry)

    ##
    # \brief Generates the refresh interval SOD entry
    def _make_refresh_interval_of_reset_guarding_sod_entry(self):
        #0x100E / 0x00
        sodentry = SODDefaultEntries.get_default_sodentries_for_object(
                        SODStandardObjects.RefreshIntervalResetGuarding, 0)
        self.processingdata.sodentries.extend(sodentry)

    ##
    # \brief Generates the device vendor information SOD entries
    def _make_device_vendor_information_sod_entries(self):
        #0x1018 / 00
        sodentries = SODDefaultEntries.get_default_sodentries_for_object(
                        SODStandardObjects.DeviceVendorInformation, 0)
        self.processingdata.sodentries.extend(sodentries)

    ##
    # \brief Generates the unique device id SOD entry
    def _make_unique_device_id_sod_entry(self):
        #0x1019 / 00
        sodentry = SODDefaultEntries.get_default_sodentries_for_object(
                        SODStandardObjects.UniqueDeviceId, 0)
        self.processingdata.sodentries.extend(sodentry)

    ##
    # \brief Genereates the parameter download SOD entry
    def _make_parameter_download_sod_entry(self):
        #0x101A / 00
        sodentry = SODDefaultEntries.get_default_sodentries_for_object(
                        SODStandardObjects.ParameterDownload, 0)
        self.processingdata.sodentries.extend(sodentry)

    ##
    # \brief Generates the common communication parameter SOD entries
    def _make_common_communication_parameter_sod_entries(self):
        #0x1200 / 0x00
        sodentries = SODDefaultEntries.get_default_sodentries_for_object(
                        SODStandardObjects.CommonCommunicationParameters, 0)
        self.processingdata.sodentries.extend(sodentries)

    ##
    # \brief Generates the mapping SOD entries for RxSPDO and TxSPDO
    def _make_mapping_sod_entries(self):

        attrlist = [SODAttributes.RW, SODAttributes.CRC, SODAttributes.BEF_WR]

        #RxSPDO mapping Parameter
        objectindex = SODStandardObjects.RxSPDOMappingParameter.start
        self._make_mapping_sod_entries_for_transportlist(
                self.processingdata.spdotransport_output, objectindex, attrlist)

        #TxSPDO mapping Parameter
        objectindex = SODStandardObjects.TxSPDOMappingParameter.start
        self._make_mapping_sod_entries_for_transportlist(
                self.processingdata.spdotransport_input, objectindex, attrlist)

    ##
    # \brief Generates Tx communication parameter SOD entries
    def _make_tx_com_sod_entries(self):
        #0x1C00
        objectoffset = 0

        for _ in self.processingdata.spdotransport_input:
            sodentries = SODDefaultEntries.get_default_sodentries_for_object(
                SODStandardObjects.TxSPDOCommunicationParameter, objectoffset)
            self.processingdata.sodentries.extend(sodentries)
            objectoffset += 1

    ##
    # \brief Generates Rx communication parameter SOD entries
    def _make_rx_com_sod_entries(self):
        #0x1400
        objectoffset = 0

        for _ in self.processingdata.spdotransport_output:
            sodentries = SODDefaultEntries.get_default_sodentries_for_object(
                SODStandardObjects.RxSPDOCommunicationParameter, objectoffset)
            self.processingdata.sodentries.extend(sodentries)
            objectoffset += 1

    ##
    # \brief Generates Structure objects for settings which are part of the
    # parameter stream
    def _generate_settings_structs_paramstream(self):

        sgstructmembers = []

        #generate structures for parameters
        for settingsgroup in self.processingdata.settingsgroups:

            structmembers = self.__generate_settings_structs_paramstream_inner(
                                                        settingsgroup.settings)

            if len (structmembers) == 0:
                continue

            typename = ''.join(['t', settingsgroup.id, 'ParamStreamPos'])
            comment = ''.join(['Structure for the SettingsGroup "',
                               settingsgroup.id, '"'])
            struct = Struct(typename, structmembers, settingsgroup.id, comment)
            self.processingdata.settings_structs_paramstream.append(struct)

            sgstructmembers.append(struct)

        if len(sgstructmembers) == 0:
            return

        #generate structure which groups all parameter structures
        sgtypename = 'tSettingsGroups'
        sgcomment = 'Structure of all SettingsGroups'

        sgstruct = Struct(sgtypename, sgstructmembers,
                          'manParams', comment=sgcomment)
        self.processingdata.settingsgroups_struct_paramstream = sgstruct

    ##
    # \brief Generates Structure objects and SOD entries for settings
    # which have a SOD position
    #
    # This method generates SODEntry objects and related Structmember objects
    # which represent the actual object data for Settings elements with an
    # SOD position of a.xosdd file. The
    # The members will be grouped inside a Structure object. This object
    # will be stored in the converters self.settings_sodpos_struct
    def _generate_settings_sodpos_struct_and_according_sod_entries(self):

        settingslist = []
        attrlist = [SODAttributes.RW]

        #get all settings of all settings groups into a list
        for settingsgroup in self.processingdata.settingsgroups:
            settingslist.extend(settingsgroup.settings)

        sod_pos_settings = self._group_settingslist_by_sod_positions(
                                                                settingslist)

        if len(sod_pos_settings) == 0:
            return

        structidentifier = 'SettingsSODPos'
        structcomment = 'Structure for the Settings with SODPosition'
        typename = 'tSettingsSODPos'
        struct = Struct(typename, identifier=structidentifier,
                        comment=structcomment)

        #generate structs and SOD entries
        for sod_pos in sod_pos_settings:

            settingslist = sod_pos_settings[sod_pos]
            setting = settingslist[0]

            indexstr = get_identifier_index_string(sod_pos[0])
            subindexstr = get_identifier_subindex_string(sod_pos[1])

            identifier, osddvaluetype = \
                self._get_generic_identifier_from_setting(
                                settingslist, setting, indexstr, subindexstr)

            osddvaluetype = self._change_datatype_if_boolean(osddvaluetype)

            membercomment = ' '.join(['Settings object data',
                                      indexstr, subindexstr])
            #generate struct
            self._make_structmember_translate_datatype_add_to_list(
                membercomment, struct, identifier, osddvaluetype)

            #generate SOD entry
            self._make_sodentry_translate_datatype_add_to_list(
                struct, attrlist[:], None, sod_pos, identifier, osddvaluetype,
                self.processingdata.sodentries)

        #create SOD entries for subindexes 0:
        comment = 'Settings with SOD Position'
        self._make_num_entries_sod_entries(
                    comment, sod_pos_settings, self.processingdata.sodentries)

        self.processingdata.settings_sodpos_struct = struct

    ##
    # \brief Generates vendor module specific SOD entries
    def _make_vendor_module_specific_sod_entries(self):
        #0x2000 / 0x00
        # copy entry, only if parameterstream settings
        if self.processingdata.settings_structs_paramstream:
            sodentries = SODDefaultEntries.get_default_sodentries_from_to(
                            0x2000, 0x2000, 0)
            self.processingdata.sodentries.extend(sodentries)

        #0x2001 / 0x00
        sodentries = SODDefaultEntries.get_default_sodentries_from_to(
                        0x2001, 0x2001, 0)
        self.processingdata.sodentries.extend(sodentries)

    ##
    # \brief Generates the end of SOD entry
    def _make_end_sod_entry(self):
        #SOD_k_END_OF_THE_OD / 0xFF
        self.processingdata.sodentries.append(SODDefaultEntries.get_end_entry())

    ##
    # \brief Groups objects with same SOD position and saves the information
    # to the given dictionary
    #
    # \param obj Object to examine
    # \param ordereddict OrderedDictionary to insert the object
    def __group_sod_postions_inner(self, obj, ordereddict):
        if obj.sodindex:
            pos = (int(obj.sodindex, NumberSystemBases.HEX) ,
                   int(obj.sodsubindex, NumberSystemBases.HEX))
            if pos not in ordereddict:
                ordereddict[pos] = []
            ordereddict[pos].append(obj)

    ##
    # \brief Groups the settings of a given list by SOD positions and
    # stores the information to a dictionary.
    #
    # \param settingslist List with Setting objects
    # \return dictionary with the settings grouped by SOD position
    def _group_settingslist_by_sod_positions(self, settingslist):
        sod_pos_settings = OrderedDict()
        for setting in settingslist:
            self.__group_sod_postions_inner(setting, sod_pos_settings)
        return sod_pos_settings

    ##
    # \brief Groups channels of a given SPDO Transport object by SOD positions
    #
    # \param transport SPDO Transport object
    # \return dictionary with the channels grouped by SOD position
    def _group_channels_to_transports(self, transport):
        sod_pos_channels = OrderedDict()
        for channel in transport.channels:
            self.__group_sod_postions_inner(channel, sod_pos_channels)
        return sod_pos_channels

    ##
    # \brief Returns the biggest datatype used in a list of channels or settings
    #
    # \param channel_or_settingslist List with channels or settings for which the
    # biggest type will be determined
    # \return biggest datatype found in the list
    def _get_biggest_datatype_of_channel_setting(self, channel_or_settingslist):
        biggest_type = None

        for channel_or_setting in channel_or_settingslist:
            #determine maximum datatype
            osddvaluetype = OSDDValueTypes.get_type(channel_or_setting.datatype)
            if not osddvaluetype < biggest_type:
                biggest_type = osddvaluetype

        datatype = biggest_type
        return datatype

    ##
    # \brief Generates a macro object for single bit access
    #
    # This method generates BitAccessMacro objects for all channels in the list
    # which have boolean data type. The macro object can be used for generating
    # code for accessing single bits.
    #
    # \param channellist List with channel objects
    # \param structinstancename Instance name (identifier) of the structure
    # containing the data from the cannel
    # \param identifier Identifier of the variable with the data of the
    # channel
    def _generate_bit_access_macros(self, channellist,
                                    structinstancename, identifier):
        for channel in channellist:
            if channel.datatype == OSDDValueTypes.Boolean:
                macro = BitAccessMacro(
                        channel.id,
                        '.'.join([structinstancename, identifier]),
                        channel.transportbitmask)
                self.processingdata.bit_access_macros.append(macro)

    ##
    # \brief Gets a general identifier, biggest datatype and a counter value
    # for a list of channels
    #
    # \param channellist List with channel objects
    # \param counter_var Start counter value to use for enumerating
    # generic identifier names
    # \return Tuple with identigier, biggest datatype found and a counter value
    def _get_identifier_datatype_from_channellist(self,
                                                  channellist,
                                                  counter_var):

        identifier = None
        datatype = None

        if len(channellist) > 1:

            datatype = self._get_biggest_datatype_of_channel_setting(
                                                                channellist)

            #use a generic identififer name, if multiple channels share
            # a variable
            identifier = 'SafeObjectData' + '{:02d}'.format(counter_var)
            counter_var += 1

        else:
            channel = channellist[0]
            identifier = channel.id
            datatype = channel.datatype

        return identifier, datatype, counter_var

    ##
    # \brief Changes the OSDD datatype to 'Uint8' if it is 'Boolean'
    #
    # \param osddvaluetype OSDDValueType to convert
    # \return 'Uint8' if passed data type is 'Boolean'; else no change
    def _change_datatype_if_boolean(self, osddvaluetype):
        if osddvaluetype == OSDDValueTypes.Boolean:
            return OSDDValueTypes.UInt8
        return osddvaluetype

    ##
    # \brief Generates SOD entries with information about the number of
    # subindexes
    #
    # \param comment Comment for the SODEntry object
    # \param sod_pos_dict Dictionary with SOD positions (index, subindex) tuples
    # as keys
    # \param sodentrieslist List to which the new entry object will be stored
    def _make_num_entries_sod_entries(self, comment,
                                      sod_pos_dict, sodentrieslist):
        #create sod entries for subindexes 0:
        sodindexes = list(set([obj[0] for obj in sod_pos_dict]))
        for sodindex in sodindexes:
            if (sodindex, 0) not in sod_pos_dict:
                sodentry = SODEntryNumEntries(sodindex, 0, comment)
                sodentrieslist.append(sodentry)

    ##
    # \brief Generates a structure member and adds it to the given struct
    #
    # This method generates a structure member with a TargetDatatype
    # translated from a OSDDValueType, comment and identifier and
    # adds it to the structmemberlist of the specified struct.
    #
    # \param comment Comment for struct member
    # \param struct Structure object to add the new member to
    # \param identifier Identifier of the structure member
    # \param datatype Data type (OSDDValueType) which will get
    # translated to an appropriate TargetDataType
    def _make_structmember_translate_datatype_add_to_list(self, comment, struct,
                                                          identifier, datatype):

        memberdatatype = OSDDValueType_TargetDataType.get_TargetDataType(
                                                                    datatype)
        membercomment = comment
        member = Structmember(memberdatatype, identifier,
            membercomment)
        struct.members.append(member)

    ##
    # \brief Generates an SOD entry, with SODDatatype, attributes,
    # and object data reference identifier
    #
    # \param struct Structure object which contains the object data
    # \param attrlist SODAttributes as list
    # \param defval Reference to a default value
    # \param sod_pos SOD Position (index, subindex) tuple
    # \param identifier Identifier of the struct member which holfs the actual
    # data
    # \param osdd_datatype OSDDValueType which will get translated to an
    # appropriate SODDataType
    # \param sodentrylist List to which the new SODEntry object will be stored
    def _make_sodentry_translate_datatype_add_to_list(self, struct,
                                                      attrlist, defval,
                                                      sod_pos, identifier,
                                                      osdd_datatype,
                                                      sodentrylist):

        soddatatype = OSDDValueType_SODDataType.get_SODDataType(osdd_datatype)
        sodattr = SODAttr(attrlist, soddatatype, None, defval)
        objectdataref = '.'.join([struct.identifier, identifier])
        sodentry = SODEntry(sod_pos[0], sod_pos[1], sodattr, objectdataref,
                            None, SODCallback.SOD_k_NO_CALLBACK)
        sodentrylist.append(sodentry)

    ##
    # \brief Generates the mapping SOD entries for the Transport objects in the
    # given list, for a specified SOD index and SOD attribute list
    #
    # \param transportlist List with Transport objects
    # \param objectindex Index of SOD
    # \param attrlist List of SODAttributes for the generated SOD entries
    def _make_mapping_sod_entries_for_transportlist(self, transportlist,
                                                    objectindex, attrlist):
        indexoffset = 0
        subindex = 0

        for transport in transportlist:
            subindex = 0
            sodindex = indexoffset + objectindex

            sod_positions = self._group_channels_to_transports(transport)
            numberofmappings = len(sod_positions)

            #make subindex 0 SOD entry
            soddatatype = SODDataTypes.U8

            defval = SODDefaultValue(value='0x00',
                                     sektor=SODDefines.SAFE_INIT_SEKTOR,
                                     process_later=True)
            sodattr = SODAttr(attrlist, soddatatype, None, defval)

            arange = SODRange(None, None, 0, numberofmappings)

            #first mapping entry
            sodentry = None
            if transport.direction == OSDDDirections.Input:
                comment = SODIndexComments.TxSpdoMappingParameter
                objectdataref = ''.join(['s_', IdentifierPlaceholder.INSTANCE,
                                         '_act_general.aTxMapParam[',
                                         str(indexoffset),
                                         '].ucNoMappingEntries'])
                sodentry = SODEntry(sodindex, subindex, sodattr, objectdataref,
                                    arange,
                                    SODCallback.SPDO_SOD_TxMappPara_CLBK,
                                    comment)
            else:
                comment = SODIndexComments.RxSpdoMappingParameter
                objectdataref = ''.join(['s_', IdentifierPlaceholder.INSTANCE,
                                         '_act_general.aRxMapParam[',
                                         str(indexoffset),
                                         '].ucNoMappingEntries'])
                sodentry = SODEntry(sodindex, subindex, sodattr, objectdataref,
                                    arange,
                                    SODCallback.SPDO_SOD_RxMappPara_CLBK,
                                    comment)

            self.processingdata.sodentries.append(sodentry)

            subindex += 1
            mappingindex = 0
            defval = None
            soddatatype = SODDataTypes.U32

            #mapping entries at subindex 0x1 and above
            for _ in range(numberofmappings):

                if not defval:
                    defval = SODDefaultValue (
                        value='0x00000000UL',
                        sektor=SODDefines.SAFE_INIT_SEKTOR,
                        process_later=True)
                sodattr = SODAttr(attrlist, soddatatype, None, defval)

                if transport.direction == OSDDDirections.Input:
                    objectdataref = ''.join(['s_',
                                             IdentifierPlaceholder.INSTANCE,
                                             '_act_general.aTxMapParam[',
                                             str(indexoffset),
                                             '].aulMappingEntry[',
                                             str(mappingindex), ']'])
                    sodentry = SODEntry(sodindex, subindex, sodattr,
                                        objectdataref, None,
                                        SODCallback.SPDO_SOD_TxMappPara_CLBK)
                else:
                    objectdataref = ''.join(['s_',
                                             IdentifierPlaceholder.INSTANCE,
                                             '_act_general.aRxMapParam[',
                                             str(indexoffset),
                                             '].aulMappingEntry[',
                                             str(mappingindex), ']'])
                    sodentry = SODEntry(sodindex, subindex, sodattr,
                                        objectdataref, None,
                                        SODCallback.SPDO_SOD_RxMappPara_CLBK)

                self.processingdata.sodentries.append(sodentry)
                subindex += 1
                mappingindex += 1

            indexoffset += 1

    ##
    # \brief Generates struct members for settings which are part of the
    # parameter stream
    #
    # \param settingslist List with Setting objects
    # \return List with Structmember objects
    def __generate_settings_structs_paramstream_inner(self, settingslist):
        counter = 0
        structmembers = []

        for setting in settingslist:

            #skip settings with SOD position
            if setting.sodindex:
                continue

            osddvaluetype = self._change_datatype_if_boolean(setting.datatype)

            datatype = \
                OSDDValueType_TargetDataType.get_TargetDataType(osddvaluetype)

            identifier = setting.id

            if not re.match(RegExPatterns.VARIABLE_IDENTIFIER, setting.id):
                if setting.name and \
                re.match(RegExPatterns.VARIABLE_IDENTIFIER, setting.name):
                    identifier = setting.name
                else:
                    identifier = ''.join(['SettingsData', str(counter)])
                    counter += 1

            member = Structmember(datatype, identifier, None)
            structmembers.append(member)
        return structmembers

    ##
    # \brief Returns a generic identifier string for multiple settings
    # sharing a common sod position (index, subindex)
    #
    # \param settingslist List with settings
    # \param setting Setting object from osdd_module, usually first setting
    # of settingslist
    # \param indexstr String representation of SOD index
    # \param subindexstr String representation of SOD subindex
    # \return Tuple with generated identifier and the OSDDValueType used
    def _get_generic_identifier_from_setting(self, settingslist, setting,
                                             indexstr, subindexstr):
        generic_identifier = ''.join(['SettingsData_', indexstr,
                                      '_', subindexstr])
        osddvaluetype = None
        identifier = setting.id

        if len(settingslist) > 1:
            identifier = generic_identifier
            osddvaluetype = self._get_biggest_datatype_of_channel_setting(
                                                                settingslist)
        else:
            osddvaluetype = setting.datatype

        if not re.match(RegExPatterns.VARIABLE_IDENTIFIER, setting.id):
            if hasattr(setting, 'name') and \
             re.match(RegExPatterns.VARIABLE_IDENTIFIER, setting.name):
                identifier = setting.name
            else:
                identifier = generic_identifier

        return identifier, osddvaluetype

##
# \}
# \}
