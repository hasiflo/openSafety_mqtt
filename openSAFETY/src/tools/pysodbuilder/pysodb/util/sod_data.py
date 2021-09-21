##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file util/sod_data.py
# Data structures close to SOD abstraction
#
# This file contains classes which are used specific SOD related parts

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
from pysodb.core.errorhandler import ErrorHandler


##
# \brief Represents a list of SODEntries
#
# This class is a subclass of Pyhton's standard list and is used specifically
# for SODEntry objects
class SODEntriesList(list):

    ##
    # \brief Initialises the object
    def __init__(self, *args, **kwargs):
        super(SODEntriesList, self).__init__(*args, **kwargs)

    ##
    # \brief Returns a list of SODEntry objects
    #
    # This function returns a list of SODEntry objects within the given range.
    # \param sodobjectrange SODIndexRange object which is an attribute of
    # SODStandardObjects
    # \param append_endentry appends the SODEntryEnd object, if True; no
    # appendance otherwise
    # \return List of SODEntry objects
    def get_entries_for_object(self, sodobjectrange, append_endentry=False):
        return self.get_entries_from_to(sodobjectrange.start,
                                        sodobjectrange.end,
                                        append_endentry)

    ##
    # \brief Returns a list of SODEntry objects
    #
    # This function returns a list of SODEntry objects within the given range.
    # \param startindex index of the SOD from where on SODEntries will be
    # returned
    # \param endindex_inclusive index of the SOD to where SODEntries will be
    # returned (inclusive)
    # \param append_endentry appends the SODEntryEnd object, if True; no
    # appendance otherwise
    # \return New list with SOD entries sorted ascending by SOD index and
    # subindex
    def get_entries_from_to(self, startindex, endindex_inclusive,
                            append_endentry=False):
        result = SODEntriesList()
        for entry in self:
            if isinstance(entry.objectindex, int):
                if entry.objectindex >= startindex and \
                entry.objectindex <= endindex_inclusive:
                    result.append(entry)
            elif isinstance(entry, SODEntryEnd) and append_endentry:
                result.append(entry)

        result.sort()
        return result


##
# \brief Represents an SOD Attribute with attributes, datatype, maxmium object
# length and a reference to a default value.
class SODAttr(object):

    ##
    # \brief Initialises the object
    #
    # \param attribute_list list of SODAttributes
    # \param soddatatype datatype of the SOD entry value
    # \param maxobj_len string with information about the maximum object length
    # \param defaultval_ref reference to a SODDefaultValue object
    def __init__(self, attribute_list, soddatatype, maxobj_len, defaultval_ref):
        self.attributes = attribute_list
        self.datatype = soddatatype
        self.max_length = maxobj_len
        self.defaultval_ref = defaultval_ref


##
# \brief Represents a SOD entry
class SODEntry(object):

    ##
    # \brief Initialises an SODEntry object
    #
    # \param sodindex index of the SOD entry
    # \param sodsubindex subindex of the SOD entry
    # \param attributes SODAttr object
    # \param objectdata_ref reference to the actual object data, either a string
    # or a SOD_ActLenPtrData object
    # \param range_ref reference to a SODRange object
    # \param callback identifier of the callback function to use
    # \param comment comment for the SOD entry
    # \param sodindexalias string alias for an SOD index, this can be used
    # when working with defines for describing an index
    # \param subindexalias string alias for an SOD subindex, this can be used
    # when working with defines for describing an index
    def __init__(self, sodindex, sodsubindex, attributes, objectdata_ref,
                 range_ref, callback, comment=None, sodindexalias=None,
                 subindexalias=None):

        self.objectindex = sodindex
        self.subindex = sodsubindex
        self.attributes = attributes
        self.objectdataref = objectdata_ref
        self.rangeref = range_ref
        self.callback = callback
        self.comment = comment
        self.indexalias = sodindexalias
        self.subindexalias = subindexalias

    ##
    # \brief Implements the less-than-comparison for SODEntry objects
    #
    # This function implements the less-than- comparison for SODEntry
    # objects. If it is compared with a regular SODEntry object, the comparison
    # is based on the values of the SOD indexes, or subindexes if former are
    # equal. If it is compared with a SODEntryEnd object, the function will
    # always return True. This ensures that the SODEntryEnd object is
    # recognised as the biggest value when sorting a list with SODEntry objects.
    def __lt__(self, other):
        if other.objectindex is None:
            return True
        else:
            return ((self.objectindex, self.subindex) <
                        (other.objectindex, other.subindex))

    ##
    # \brief Implements the greater-than-comparison for SODEntry objects
    #
    # This function implements the greater-than-comparison for SODEntry
    # objects. If it is compared with a regular SODEntry object, the comparison
    # is based on the values of the SOD indexes, or subindexes if former are
    # equal. If it is compared with a SODEntryEnd object, the function will
    # always return False. This ensures that the SODEntryEnd object is
    # recognised as the biggest value when sorting a list with SODEntry objects.
    def __gt__(self, other):
        if other.objectindex is None:
            return False
        else:
            return ((self.objectindex, self.subindex) >
                        (other.objectindex, other.subindex))


##
# \brief Represents an SOD entry with information about the number of
# follwing sub indexentries
#
# This class is a subclass of SODEntry and represents an SOD entry with
# information about the nubmer of following subindex entries.
# SODEntryNumEntries are usually used and located at subindexes 0x00.
class SODEntryNumEntries(SODEntry):
    def __init__(self, sodindex, sodsubindex, comment=None):
        super(SODEntryNumEntries, self).__init__(
            sodindex, sodsubindex,
            SODAttr([SODAttributes.CONS], SODDataTypes.U8, None, None),
            None, None, SODCallback.SOD_k_NO_CALLBACK, comment)

    ##
    # \brief Sets the references of actual and default value to an identifier
    # with the given number.
    #
    # \param number number which should be used in the identifier name for
    # actual and default value references.
    def set_objectdata_and_defaultval_references_noE(self, number):
        identifier = ''.join(['b_noE_', str(number)])
        self.objectdataref = identifier
        self.attributes.defaultval_ref = SODDefaultValue(value=str(number),
                                                         identifier=identifier)


##
# \brief Represents the end of SOD
#
# This class is a subclass of SODEntry and represents the end of SOD
class SODEntryEnd(SODEntry):

    def __init__(self):
        super(SODEntryEnd, self).__init__(
            None, 0xFF, SODAttr(['0'], SODDataTypes.EPLS_k_BOOLEAN, None, None),
            None, None, SODCallback.SOD_k_NO_CALLBACK,
            SODIndexComments.EndOfSOD, sodindexalias='SOD_k_END_OF_THE_OD')

    ##
    # \brief Implements the less-than- comparison for SODEntryEnd objects
    #
    # This function implements the less-than- comparison for SODEntryEnd
    # objects. If it is compared with a regular SODEntry object, the return
    # value is always False. This ensures that this object is recognised as the
    # biggest value when sorting a list with SODEntry objects.
    def __lt__(self, other):
        if isinstance(other, (SODEntry, SODEntryEnd)):
            return False

    ##
    # \brief Implements the greater-than- comparison for SODEntryEnd objects
    #
    # This function implements the greater-than- comparison for SODEntryEnd
    # objects. If it is compared with a regular SODEntry object, the return
    # value is always True. This ensures that this object is recognised as the
    # biggest value when sorting a list with SODEntry objects.
    def __gt__(self, other):
        if isinstance(other, SODEntryEnd):
            return False
        elif isinstance(other, SODEntry):
            return True


##
# \brief Class which representes a variable
class Variable(object):

    ##
    # \brief Initialises the object
    # \param datatype data type of the default value
    # \param identifier identifier of the default value at variable declaration.
    # this is also used for SOD entries and their references to default values
    # \param value value
    # \param sektor sektor which is used at variable declaration / initialisation
    # \param arraysize size of the array, of the default value
    # \param process_later flag which indicates that a later processing is
    # required, if True; no later processing otherwise
    def __init__(self, datatype=None, identifier=None, value=None, sektor=None,
                 arraysize=None, process_later=None):
        self.datatype = datatype
        self.identifier = identifier
        self.value = value
        self.sektor = sektor
        self.arraysize = arraysize
        self.process_later = process_later
        self.used_for_generation = False


##
# \brief Class which represents a default value a SOD entry can have
class SODDefaultValue(Variable):
    def __init__(self, *args, **kwargs):
        super(SODDefaultValue, self).__init__(*args, **kwargs)

        if not self.value:
            message = 'SODDefaultValue: no value specified!'
            ErrorHandler.warning(message)


##
# \brief Class which represents variable used in SOD
class SODVariable(Variable):
    pass


##
# \brief Class which represents in which range an actual value of an SOD entry
# can be
class SODRange(object):

    ##
    # \brief Initialises the object
    #
    # \param datatype Datatype of the values low and high
    # \param identifier Identifier of the range at variable declaration
    # \param low lower bound value of the range
    # \param high Upper bound value of the range
    def __init__(self, datatype, identifier, low, high):
        self.datatype = datatype
        self.identifier = identifier
        self.low = low
        self.high = high


##
# \brief Groups SOD data types used in sod.c for SOD entries
class SODDataTypes(object):
    U8 = 'U8'
    U16 = 'U16'
    U32 = 'U32'
    I8 = 'I8'
    I16 = 'I16'
    I32 = 'I32'
    OCT = 'OCT'
    DOM = 'DOM'
    EPLS_k_BOOLEAN = 'EPLS_k_BOOLEAN'

    B8 = 'B8'
    I64 = 'I64'
    U64 = 'U64'
    R32 = 'R32'
    R64 = 'R64'
    VSTR = 'VSTR'


##
# \brief Groups SOD Attribute constants
class SODAttributes(object):
    CONS = 'CONS'
    RO = 'RO'
    RW = 'RW'
    WO = 'WO'
    CRC = 'CRC'
    PDO = 'PDO'
    NLEN = 'NLEN'
    BEF_RD = 'BEF_RD'
    BEF_WR = 'BEF_WR'
    AFT_WR = 'AFT_WR'


##
# \brief Groups comment text for several SOD indexes
class SODIndexComments(object):
    InputData = 'Input Data'
    OutputData = 'Output Data'
    TxSpdoMappingParameter = 'TxSPDO mapping Parameter'
    TxSpdoComParameter = 'TxSPDO communication parameters'
    RxSpdoMappingParameter = 'RxSPDO mapping Parameter'
    RxSpdoComParameter = 'RxSPDO communication parameters'
    ErrorStatistics = 'Error statistics'
    ErrorRegister = 'Error Register'
    LifeGuarding = 'Life Guarding'
    NumberOfRetriesResetGuarding = 'Pre-Operational signal, Number of Retries '\
                                   'for Reset Guarding'
    RefreshIntervalResetGuarding = 'Refresh interval of Reset Guarding'
    DeviceVendorInformation = 'Device Vendor Information'
    UniqueDeviceId = 'Unique Device ID'
    ParameterDownload = 'Parameter download'
    CommonCommunicationParameter = 'Common Communication Parameter'
    VendorModuleSpecificEntries = 'vendor/module specific data'
    EndOfSOD = 'end of SOD'


##
# \brief Groups constants used in SOD entries
class SODDefines(object):
    NULL = 'NULL'
    SAFE_INIT_SEKTOR = 'SAFE_INIT_SEKTOR'
    SAFE_NO_INIT_SEKTOR = 'SAFE_NO_INIT_SEKTOR'
    SOD_k_END_OF_THE_OD = 'SOD_k_END_OF_THE_OD'


##
# \brief Groups constants for SOD callback identifiers
class SODCallback(object):
    SPDO_SOD_TxMappPara_CLBK = 'SPDO_SOD_TxMappPara_CLBK'
    SPDO_SOD_RxMappPara_CLBK = 'SPDO_SOD_RxMappPara_CLBK'
    SAPL_SOD_ParameterSet_CLBK = 'SAPL_SOD_ParameterSet_CLBK'
    SHNF_SOD_ConsTimeBase_CLBK = 'SHNF_SOD_ConsTimeBase_CLBK'
    SOD_k_NO_CALLBACK = 'SOD_k_NO_CALLBACK'


##
# \brief Represents an range of SOD indexes
class SODIndexRange(object):
    def __init__(self, startindex, endindex_inclusive=None):
        self.start = startindex

        if not endindex_inclusive:
            self.end = startindex
        else:
            self.end = endindex_inclusive


##
# \brief Groups the SOD index ranges an SOD's default structure
class SODStandardObjects(object):
    ErrorRegister = SODIndexRange(0x1001)
    ManufacturerStatusRegister = SODIndexRange(0x1002)
    PredefinedErrorField = SODIndexRange(0x1003)
    ErrorStatistics = SODIndexRange(0x1004)
    LifeGuarding = SODIndexRange(0x100C)
    NumberOfRetriesResetGuarding = SODIndexRange(0x100D)
    RefreshIntervalResetGuarding = SODIndexRange(0x100E)
    DeviceVendorInformation = SODIndexRange(0x1018)
    UniqueDeviceId = SODIndexRange(0x1019)
    ParameterDownload = SODIndexRange(0x101A)
    SCMSpecificParameters = SODIndexRange(0x101B)
    CommonCommunicationParameters = SODIndexRange(0x1200)
    SSDOCommunicationParameters = SODIndexRange(0x1201)
    SNMTCommunicationParameters = SODIndexRange(0x1202)
    RxSPDOCommunicationParameter = SODIndexRange(0x1400, 0x17FE)
    RxSPDOMappingParameter = SODIndexRange(0x1800, 0x1BFE)
    TxSPDOCommunicationParameter = SODIndexRange(0x1C00, 0x1FFE)
    UserParameter = SODIndexRange(0x2800, 0x2FFF)
    TxSPDOMappingParameter = SODIndexRange(0xC000, 0xC3FE)
    SadrDviList = SODIndexRange(0xC400, 0xC7FE)
    AdditionalSadrList = SODIndexRange(0xC801, 0xCBFF)
    SadrUdidList = SODIndexRange(0xCC01, 0xCFFF)
    AdditionalParameterList = SODIndexRange(0xE400, 0xE7FE)
    SODEnd = SODIndexRange(0xFFFF)

##
# \}
# \}
