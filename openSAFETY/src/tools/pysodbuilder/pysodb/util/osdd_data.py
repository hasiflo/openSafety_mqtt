##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file util/osdd_data.py
# Data structures close to OSDD abstraction
#
# This file contains classes which are used for abstracting specific
# OSDD related parts

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


##
# \brief Abstracts an OSDD Channel element
class Channel(object):
    def __init__(self, channelid, datatype, transportname,
                 transportindex, transportbitmask, sodindex, sodsubindex,
                 activeinfunctions, channelsettings):
        self.id = channelid
        self.datatype = datatype
        self.transportname = transportname
        self.transportindex = transportindex
        self.transportbitmask = transportbitmask
        self.sodindex = sodindex
        self.sodsubindex = sodsubindex
        self.activeinfunctions = activeinfunctions
        self.settings = channelsettings


##
# \brief Abstracts an OSDD ChannelChannelGroup element
class ChannelGroup(object):
    def __init__(self, channelgroupid, channels):
        self.id = channelgroupid
        self.channels = channels


##
# \brief Abstracts an OSDD Setting element
class Setting(object):
    def __init__(self, datatype, settingid, settingname, paramstreampos,
                 templateid, sodindex, sodsubindex):
        self.datatype = datatype
        self.id = settingid
        self.name = settingname
        self.paramstreampos = paramstreampos
        self.templateid = templateid
        self.sodindex = sodindex
        self.sodsubindex = sodsubindex


##
# \brief Abstracts an OSDD SettingsGroup element
class SettingsGroup(object):
    def __init__(self, groupid, settings, copyfromchannel=None):
        self.id = groupid
        self.settings = settings
        self.copyfromchannel = copyfromchannel


##
# \brief Abstracts an OSDD Function element
class Function(object):
    def __init__(self, funcid, subfunctions=None):
        self.id = funcid
        self.subfunctions = subfunctions

        if subfunctions is None:
            self.subfunctions = []


##
# \brief Abstracts an OSDD SettingDataTemplate element
class SettingDataTemplate(object):
    def __init__(self, sdtid, datatype):
        self.id = sdtid
        self.datatype = datatype

##
# \brief Returns a sorted list of Settings where its OSDDVisibility attribute
# is not 'None'
#
# \param osdd_settings List of Setting elements from an OSDD module
# \return Sorted list with Setting elements; settings with
# ParameterStream / Position (ELEMENT/ATTRIBUTE) first and
# settings without such a position, but according to their occurence
# in the OSDD file, afterwards.
def get_sorted_settings_list(osdd_settings):

    settings_without_paramstreampos = []
    settings_with_paramstreampos = []

    for setting in osdd_settings:

        settingtype = None
        settingid = None
        settingname = None
        paramstreampos = None
        settingtemplate = None
        settingsodindex = None
        settingsodsubindex = None

        # skip setting, if OSDDVisibility attribute in OSDD states, that
        # the value will not be transmitted to the module
        if setting.Visibility == OSDDVisibility.None_:
            continue

        settingid = setting.id

        if  hasattr(setting, 'Name') and setting.Name:
            settingname = setting.Name

        if setting.Type:
            settingtype = setting.Type

        if setting.DataSelection:
            settingtype = setting.DataSelection.Type

        if setting.ParameterStream:
            paramstreampos = setting.ParameterStream.Position
        else:
            paramstreampos = None

        if setting.Template:
            settingtemplate = setting.Template

        if setting.SODPosition:
            settingsodindex = setting.SODPosition.Index
            settingsodsubindex = setting.SODPosition.SubIndex

        asetting = Setting(settingtype, settingid, settingname, paramstreampos,
                           settingtemplate, settingsodindex,
                           settingsodsubindex)

        if asetting.paramstreampos:
            settings_with_paramstreampos.append(asetting)
        else:
            settings_without_paramstreampos.append(asetting)

    settings_with_paramstreampos.sort(key=lambda s:s.paramstreampos)

    settingslist = settings_with_paramstreampos \
                    + settings_without_paramstreampos

    return settingslist


##
# \brief Abstracts an OSDD Transport element
class Transport(object):
    def __init__(self, name, direction, octetsize):
        self.name = name
        self.direction = direction
        self.octetsize = octetsize


##
# \brief  Represents a SPDO Transport
class SPDOTransport(Transport):
    def __init__(self, name, direction, octetsize):
        Transport.__init__(self, name, direction, octetsize)
        self.channels = []


##
# \brief Represents all possible SettingDataTemplate elements
class OSDDSettingDataTemplateElements(object):
    Choice = 'Choice'
    Range = 'Range'
    ParameterConstants = 'ParameterConstants'


##
# \brief Represents all possible OSDDVisibility elements
class OSDDVisibility(object):
    Overall = 'Overall'
    ReadOnly = 'ReadOnly'
    SafetyOnly = 'SafetyOnly'

    # 'None_' because 'None' is a keyword
    None_ = 'None'


##
# \brief Constants for OSDD directions
class OSDDDirections(object):
    Input = 'Input'
    Output = 'Output'


##
# \brief Represents an OSDD valuetype and its size
class OSDDValueType(object):
    def __init__(self, datatype, size, nextbiggertype=None):
        self.datatype = datatype
        self.size = size
        self.nextbiggertype = nextbiggertype

    def __repr__(self):
        return repr(self.datatype)

    def __eq__(self, other):
        return self.datatype == other

    ##
    # \brief Compares the size of two OSDDValueType objects
    # (operator <) and returns the result. If compared with None type,
    # true will be returned
    def __lt__(self, other):
        if other is None:
            return False
        return self.size < other.size


##
# \brief Groups OSDD value types and related sizes
class OSDDValueTypes(object):

    #boolean types
    Boolean = OSDDValueType('Boolean' , 1)

    #unsigned types
    UInt8 = OSDDValueType('UInt8', 1,)
    UInt16 = OSDDValueType('UInt16', 2)
    UInt24 = OSDDValueType('UInt24', 3)
    UInt32 = OSDDValueType('UInt32', 4)
    UInt40 = OSDDValueType('UInt40', 5)
    UInt48 = OSDDValueType('UInt48', 6)
    UInt56 = OSDDValueType('UInt56', 7)
    UInt64 = OSDDValueType('UInt64', 8)

    #signed types
    Int8 = OSDDValueType('Int8', 1)
    Int16 = OSDDValueType('Int16', 2)
    Int24 = OSDDValueType('Int24', 3)
    Int32 = OSDDValueType('Int32', 4)
    Int40 = OSDDValueType('Int40', 5)
    Int48 = OSDDValueType('Int48', 6)
    Int56 = OSDDValueType('Int56', 7)
    Int64 = OSDDValueType('Int64', 8)

    #float types
    Float32 = OSDDValueType('Float32', 4)
    Float64 = OSDDValueType('Float64', 8)

    ##
    # \brief Returns the OSSDValue type object including size for a given
    # OSDDValueType string (coming from osdd_module)
    @classmethod
    def get_type(cls, typestring):
        if typestring and hasattr(cls, typestring):
            osddvaluetype = getattr(cls, typestring)
            return osddvaluetype
        return None

    ##
    # \brief Returns the size information of a OSDDValueType object
    # for a given OSDDValueType string
    @classmethod
    def get_size_of_type(cls, typestring):
        osddtypesize = cls.get_type(typestring)
        if osddtypesize and osddtypesize.datatype == typestring:
            return osddtypesize.size
        return None


##
# \}
# \}
