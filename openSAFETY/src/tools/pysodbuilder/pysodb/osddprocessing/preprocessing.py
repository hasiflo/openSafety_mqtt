##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file osddprocessing/preprocessing.py
# Implements functionality for preprocessing an OSDD module
# like grouping channels to DataTransport elements and resolving
# OSDD data types from templates.
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
from pysodb.osddprocessing.base import OSDDProcessingBase
from pysodb.util.osdd_data import OSDDDirections, SPDOTransport, \
    OSDDSettingDataTemplateElements, SettingDataTemplate, \
    get_sorted_settings_list, Channel, ChannelGroup, Function, SettingsGroup


##
# \brief Does the preprocessing from OSDD module data to a
# form for further processing
class PreProcessing(OSDDProcessingBase):

    ##
    # \brief Starts the preprocessing of the OSDD module data
    def do_preprocessing(self):
        self._get_SPDOInfo()
        self._get_SettingDataTemplates()
        self._get_Channels()
        self._get_ModuleFunctions()
        self._get_ModuleSettings_SettingsGroups()
        self._resolve_datatypes_from_template_references()
        self._link_channelsettings_into_module_settings()
        self._link_channels_into_spdo_transports()
        self._get_used_channels()

    ##
    # \brief Gets and stores SPDO Transprort information
    def _get_SPDOInfo(self):
        spdotransports = \
            self.processingdata._channelconfiguration.DataTransport.SPDOTransport

        for st in spdotransports:
            if st.Direction == OSDDDirections.Output:
                self.processingdata.spdotransport_output.append(
                    SPDOTransport(st.Name, st.Direction, st.OctetSize))
            elif st.Direction == OSDDDirections.Input:
                self.processingdata.spdotransport_input.append(
                    SPDOTransport(st.Name, st.Direction, st.OctetSize))

    ##
    # \brief Gets and stores a list of the SettingDataTemplates of the OSDD
    # module accessed via the functionality of the OSDD bindings.
    def _get_SettingDataTemplates(self):

        sdt = self.processingdata._designconfiguration.SettingDataTemplates

        if hasattr(sdt, OSDDSettingDataTemplateElements.Choice) and sdt.Choice:
            for sdtchoice in sdt.Choice:
                item = SettingDataTemplate(sdtchoice.id, sdtchoice.Type)
                self.processingdata.settingdatatemplates.append(item)

        if hasattr(sdt, OSDDSettingDataTemplateElements.Range) and sdt.Range:
            for sdtrange in sdt.Range:
                item = SettingDataTemplate(sdtrange.id, sdtrange.Type)
                self.processingdata.settingdatatemplates.append(item)

        if hasattr(sdt, OSDDSettingDataTemplateElements.ParameterConstants) \
        and sdt.ParameterConstants:
            for sdtparamconstant in sdt.ParameterConstants.ParameterConstant:
                item = SettingDataTemplate(sdtparamconstant.id,
                                           sdtparamconstant.Type)
                self.processingdata.settingdatatemplates.append(item)

    ##
    # \brief Gets and stores ChannelGroups and related channels of the OSDD
    # module accessed via the functionality of the OSDD bindings.
    def _get_Channels(self):

        channelgroups = self.processingdata._channelconfiguration.ChannelGroup

        for channelgroup in channelgroups:

            channels = []
            channelgroupid = channelgroup.id

            for channel in channelgroup.Channel:
                datavalue = channel.DataValue

                transportname = None
                transportindex = None
                transportbitmask = None
                sodindex = None
                sodsubindex = None

                if datavalue.Transport:
                    transportname = datavalue.Transport.Name
                    transportindex = datavalue.Transport.Index
                    transportbitmask = datavalue.Transport.BitMask

                if datavalue.SODPosition:
                    sodindex = datavalue.SODPosition.Index
                    sodsubindex = datavalue.SODPosition.SubIndex

                channel_functions = []
                if channel.ActiveInFunction:
                    for function in channel.ActiveInFunction.FunctionReference:
                        channel_functions.append(function.id)


                channelid = channel.id
                datatype = datavalue.Type

                channelsettings = channel.ChannelSettings

                settingslist = []

                if channelsettings:
                    settingslist = get_sorted_settings_list(
                                    channelsettings.Setting)

                channels.append(
                    Channel(channelid, datatype,
                                transportname, transportindex, transportbitmask,
                                 sodindex, sodsubindex, channel_functions,
                                 settingslist))

            cg = ChannelGroup(channelgroupid, channels)
            self.processingdata.channelgroups.append(cg)

    ##
    # \brief Gets module functions of an OSDD module and stores the information
    # in a list / tree
    #
    # This function reads the module functions of a module and stores it
    # into a list / tree. The treewalk is done in an iterative manner.
    def _get_ModuleFunctions(self):

        modulefunctions = self.processingdata._modulefunctions

        functree = []
        rootlist = []
        childlist = []

        if modulefunctions:

            childlist = []
            for item in modulefunctions.Function:
                childlist.append((item, None))

            while True:
                rootlist = childlist[:]
                if len(rootlist) == 0:
                    break

                #clear list
                childlist[:] = []

                for functuple in rootlist:

                    function = functuple[0]
                    hasparent = functuple[1]

                    funcid = function.id
                    f = Function(funcid)

                    if (hasparent):
                        hasparent.subfunctions.append(f)
                    else:
                        functree.append(f)

                    if hasattr(function, 'Function'):
                        for child in function.Function:
                            childlist.append((child, f))

        self.processingdata.functions = functree

    ##
    # \brief Stores a list of SettingsGroups of the OSDD accessed via the
    # functionality of the OSDD bindings.
    def _get_ModuleSettings_SettingsGroups(self):

        modulesettings = self.processingdata._designconfiguration.ModuleSettings

        for settingsgroup in modulesettings.SettingsGroup:
            copyfromchannel = None
            sg = None

            if settingsgroup.CopyFromChannel and \
            settingsgroup.CopyFromChannel != 'none':
                copyfromchannel = settingsgroup.CopyFromChannel
                sg = SettingsGroup(settingsgroup.id, None, copyfromchannel)
            else:
                settingslist = get_sorted_settings_list(settingsgroup.Setting)
                sg = SettingsGroup(
                    settingsgroup.id, settingslist, copyfromchannel)

            self.processingdata.settingsgroups.append(sg)

    ##
    # \brief Resolves datatypes from references, if a template id is referenced
    def _resolve_datatypes_from_template_references(self):

        template_ids_datatypes = dict([(t.id, t.datatype) for t in \
                                    self.processingdata.settingdatatemplates])

        for channelgroup in self.processingdata.channelgroups:
            for channel in channelgroup.channels:
                for setting in channel.settings:
                    if setting.templateid:
                        setting.datatype = \
                            template_ids_datatypes[setting.templateid]

    ##
    # \brief Link settings from channels to module settings, respectively
    # settings group, if copyfromchannel is given
    #
    # This functions links the settings of the channels into the module settings
    # if they are referenced from moodule settings by a CopyFromChannel
    # attribute
    def _link_channelsettings_into_module_settings(self):

        for settingsgroup in self.processingdata.settingsgroups:
            if settingsgroup.copyfromchannel:
                channelgroup, channel = settingsgroup.copyfromchannel.split('/')

                #check if channelgroup and channel exists
                found_channelgroup = [cg for cg in \
                                      self.processingdata.channelgroups \
                                      if cg.id == channelgroup]
                if len(found_channelgroup) == 1:
                    found_channel = [c for c in found_channelgroup[0].channels \
                                     if c.id == channel]
                    if len(found_channel) == 1:
                        settingsgroup.settings = found_channel[0].settings

    ##
    # \brief Links channels to SPDO transports, so that a SPDO can be
    # processed easier
    def _link_channels_into_spdo_transports(self):
        for transport in self.processingdata.spdotransport_input \
        + self.processingdata.spdotransport_output:
            #find all related channels
            channels = []
            for channelgroup in self.processingdata.channelgroups:
                for channel in channelgroup.channels:
                    if channel.transportname == transport.name:
                        channels.append(channel)

            transport.channels = channels

    ##
    # \brief Gets the number of used channels
    def _get_used_channels(self):
        num_channels = 0
        for transport in self.processingdata.spdotransport_input \
        + self.processingdata.spdotransport_output:
            num_channels += len(transport.channels)

        self.processingdata.used_channels = num_channels

##
# \}
# \}
