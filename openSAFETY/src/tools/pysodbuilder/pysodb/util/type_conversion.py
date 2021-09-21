##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file type_conversion.py
# Functionality to convert different types to another

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
from pysodb.codegeneration.c_target_data import TargetDataTypes
from pysodb.util.osdd_data import OSDDValueTypes
from pysodb.util.sod_data import SODDataTypes


##
# \brief Translation class / table for translating SOD data types to
# C language object length information
class SODDataTypes_ObjectLength_CLang(object):

    _list = [
             (SODDataTypes.U8, '0x1UL'),
             (SODDataTypes.U16, '0x2UL'),
             (SODDataTypes.U32, '0x4UL'),

             (SODDataTypes.I8, '0x1UL'),
             (SODDataTypes.I16, '0x2UL'),
             (SODDataTypes.I32, '0x4UL'),

             (SODDataTypes.EPLS_k_BOOLEAN, '0x01UL')
             ]

    ##
    # \brief Returns a object length information as string based in the
    # given SODDataType
    #
    # \param soddatatype SODDataType to convert
    # \return Corresponding C Code object length, if given data type is valid;
    # else None
    @classmethod
    def get_ObjectLength(cls, soddatatype):
        for item in cls._list:
            if soddatatype == item[0]:
                return item[1]
        return None


##
# \brief Translation class / table for translating OSDD value types to
# C language data types
class OSDDValueType_TargetDataType(object):
    _list = [
             (OSDDValueTypes.UInt8, TargetDataTypes.UINT8),
             (OSDDValueTypes.UInt16, TargetDataTypes.UINT16),
             (OSDDValueTypes.UInt32, TargetDataTypes.UINT32),

             (OSDDValueTypes.Int8, TargetDataTypes.INT8),
             (OSDDValueTypes.Int16, TargetDataTypes.INT16),
             (OSDDValueTypes.Int32, TargetDataTypes.INT32)
             ]

    ##
    # \brief Translates a given OSDDValueType to a (C language) TargetDatatype
    #
    # \param osddvaluetype OSDDValueType to convert
    # \return Corresponding TargetDataType, if given data type is valid;
    # else None
    @classmethod
    def get_TargetDataType(cls, osddvaluetype):
        for item in cls._list:
            if osddvaluetype == item[0].datatype:
                return item[1]
        return None


##
# \brief Translation class / table for translating SOD data types to
# C language data types
class SODDataType_TargetDataType(object):

    _list = [
         (SODDataTypes.U8, TargetDataTypes.UINT8),
         (SODDataTypes.U16, TargetDataTypes.UINT16),
         (SODDataTypes.U32, TargetDataTypes.UINT32),

         (SODDataTypes.I8, TargetDataTypes.INT8),
         (SODDataTypes.I16, TargetDataTypes.INT16),
         (SODDataTypes.I32, TargetDataTypes.INT32)
         ]

    ##
    # \brief Translates a given SODDataType to a (C language) TargetDataType
    #
    # \param soddatatype SODDataType to convert
    # \return Corresponding TargetDataType, if given data type is valid;
    # else None
    @classmethod
    def get_TargetDataType(cls, soddatatype):
        for item in cls._list:
            if soddatatype == item[0]:
                return item[1]
        return None


##
# \brief Translation class / table for translating OSDD value types to
# SOD data types
class OSDDValueType_SODDataType(object):

    _list = [
             (OSDDValueTypes.UInt8, SODDataTypes.U8),
             (OSDDValueTypes.UInt16, SODDataTypes.U16),
             (OSDDValueTypes.UInt32, SODDataTypes.U32),

             (OSDDValueTypes.Int8, SODDataTypes.I8),
             (OSDDValueTypes.Int16, SODDataTypes.I16),
             (OSDDValueTypes.Int32, SODDataTypes.I32)
            ]

    ##
    # \brief Returns the appropriate SODDataType for a given OSDDValueType
    #
    # \param osddvaluetype OSDDValueType to convert
    # \return Corresponding SODDataType, if given data type is valid;
    # else None
    @classmethod
    def get_SODDataType(cls, osddvaluetype):
        for item in cls._list:
            if osddvaluetype == item[0]:
                return item[1]
        return None


##
# \brief Translation class / table for translating SOD data types to
# datatypes used for defining ranges for SOD entries
class SODDataType_SODRangeDataType(object):

    _list = [
             ([SODDataTypes.I8, SODDataTypes.U8 ], SODDataTypes.U8),
             ([SODDataTypes.I16, SODDataTypes.U16], SODDataTypes.U16),
             ([SODDataTypes.I32, SODDataTypes.U32], SODDataTypes.U32)
             ]

    ##
    # \brief Returns an appropriate SODDataType used for SODRanges for a given
    # SODDataType
    #
    # \param soddatatype SODDataType to convert
    # \return Corresponding SODDataType used for ranges, if given data type is
    # valid; else None
    @classmethod
    def get_SODRangeDataType(cls, soddatatype):
        for item in cls._list:
            if soddatatype in item[0]:
                return item[1]
        return None

    ##
    # \brief Returns all available SODDataTypes usable for SODRanges
    #
    # \return List if SODDataTypes usable for ranges
    @classmethod
    def get_SODRangeDataTypes(cls):
        result = [obj[1] for obj in cls._list]
        return result


##
# \brief Translation class / table for translating SOD data types to
# their word representation used for identifier names
class SODDataType_DataWord(object):

    _list = [
             ([SODDataTypes.U8, SODDataTypes.I8], 'b'),
             ([SODDataTypes.U16, SODDataTypes.I16], 'w'),
             ([SODDataTypes.U32, SODDataTypes.I32], 'dw')
             ]

    ##
    # \brief Returns a string specifying the given datatype, used for
    # identifier naming
    #
    # \param soddatatype SODDataType to convert
    # \return Corresponding DataWord / identifier name part, if given data type
    # is valid; else None
    @classmethod
    def get_DataWord_string (cls, soddatatype):
        for item in cls._list:
            if soddatatype in item[0]:
                return item[1]
        return None


##
# \brief Translation class / table used for translating SOD data types to
# identifier names for default values
class SODDataType_DefaultValue_Prefix(SODDataType_DataWord):

    _list = SODDataType_DataWord._list + [
         ([SODDataTypes.OCT], 'ab'),
         ([SODDataTypes.DOM], 's')
         ]

##
# \brief Translation class / table used for translating SOD data types to
# identifier names for actual data
class SODDataType_ActData_Prefix(SODDataType_DataWord):
    _list = SODDataType_DataWord._list + [
         ([SODDataTypes.OCT, SODDataTypes.DOM], 'ab')
         ]

##
# \}
# \}
