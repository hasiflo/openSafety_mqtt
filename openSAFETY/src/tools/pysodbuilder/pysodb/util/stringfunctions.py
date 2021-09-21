##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file stringfunctions.py
# Implements functionality for formatting strings and getting identifier
# strings

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
from pysodb.util.constants import NumberSystemBases, \
    CodeFormatConstants
from pysodb.util.type_conversion import SODDataType_DataWord, \
    SODDataType_DefaultValue_Prefix, SODDataType_ActData_Prefix
from pysodb.core.errorhandler import ErrorHandler


##
# \brief Returns the hex representation of a value as string
#
# \param value Value to convert to hex
# \param minimum_width Minimum width of the hexadecimal number (
#  leading zeros)
# \param leading_0x Flag which controls if the hexadecimal string should
# be preceded with '0x' (True) or not (False)
# \param lowercase_hex_letters Flag which controls the case of the hexadecimal
# digit; Uppercase (False), lowercase (True)
# \return String with the given value in hexadecimal format
def get_hex_string(value, minimum_width=0, leading_0x=False,
                   lowercase_hex_letters=False):
    if value is None:
        ErrorHandler.error('Value must not be None!')
    elif type(value) == type('') or isinstance(value, str):
        try:
            value = int(value, NumberSystemBases.HEX)
        except ValueError:
            ErrorHandler.error('Error parsing hex string to int: string does '\
                               'not represent a valid hex number')
    elif not isinstance(value, int) or (isinstance(value, int) and value < 0):
        ErrorHandler.error('The value "{}" is not valid!'.format(value))

    leading_string = ''
    case = 'X'

    if leading_0x:
        leading_string = '0x'

    if lowercase_hex_letters:
        case = 'x'

    formatstring = ''.join([leading_string, '{:0',
                            str(minimum_width), case, '}'])
    return formatstring.format(value)


##
# \brief Returns the hex presentation of a value as string with leading '0x'
#
# \param value Value to convert to hex
# \param minimum_width Minimum width of the hexadecimal number (
#  leading zeros)
# \param lowercase_hex_letters Flag which controls the case of the hexadecimal
# digit; Uppercase (False), lowercase (True)
# \return String with the given value in hexadecimal format
def get_hex_string_leading_0x(value, minimum_width=0,
                               lowercase_hex_letters=False):
    return get_hex_string(value, minimum_width, True, lowercase_hex_letters)


##
# \brief Returns the hex presentation of a value as 4 character string
# with leading '0x'
#
# \param value Value to convert to a hexadecimal string
# \return String with the given value as hexadecimal string with leading '0x'
def get_sod_index_string (value):
    return get_hex_string_leading_0x(value, CodeFormatConstants.INDEX_WIDTH)


##
# \brief Returns the hex presentation of a value as 2 character string
# with leading '0x'
#
# \param value Value to convert to a hexadecimal string
# \return String with the given value as hexadecimal string with leading '0x'
def get_sod_subindex_string (value):
    return get_hex_string_leading_0x(value, CodeFormatConstants.SUBINDEX_WIDTH)


##
# \brief Returns the hex presentation of a value as 4 character string
#
# \param value Value to convert to a hexadecimal string
# \return String with the given value as hexadecimal string without leading '0x'
def get_identifier_index_string(value):
    return get_hex_string(value, CodeFormatConstants.INDEX_WIDTH)


##
# \brief Returns the hex presentation of a value as 2 character string
#
# \param value Value to convert to a hexadecimal string
# \return String with the given value as hexadecimal string without leading '0x'
def get_identifier_subindex_string(value):
    return get_hex_string(value, CodeFormatConstants.SUBINDEX_WIDTH)


##
# \brief Returns the identifier for a value range definition
#
# \param soddatatype SOD data type to use
# \param low Lower bound of the range
# \param high Upper bound of th range
# \return String with the given value as hexadecimal string without leading '0x'
def get_struct_range_identifier(soddatatype, low, high):
    identifier = ''.join(['s_rg_',
                          SODDataType_DataWord.get_DataWord_string(soddatatype),
                          '_', str(low), '_', str(high)])
    return identifier


##
# \brief Returns the identifier for an actual data structure definition
#
# \param instance_str String which will be used for describing the belonging
# to an instance
# \param objectindex String with the SOD index
# \param subindex String with the SOD subindex
# \return String with identifier fo an actual data structure definition
def get_struct_actual_data_identifier(instance_str, objectindex, subindex):
    identifier = ''.join(['s_', instance_str, '_act_',
                          get_identifier_index_string(objectindex), '_',
                          get_identifier_subindex_string(subindex)])
    return identifier


##
# \brief Returns the identifier for a default value definition
#
# \param instance_str String which will be used for describing the belonging
# to an instance
# \param soddatatype SOD data type to use
# \param objectindex String with the SOD index
# \param subindex String with the SOD subindex
# \return String with identifier for a default value definition
def get_defval_identifier(instance_str, soddatatype, objectindex, subindex):
    identifier = ''.join([
        SODDataType_DefaultValue_Prefix.get_DataWord_string(soddatatype),
        '_', instance_str, '_def_', get_identifier_index_string(objectindex),
        '_', get_identifier_subindex_string(subindex)])
    return identifier


##
# \brief Returns the identifier for a variable definition
#
# \param instance_str String which will be used for describing the belonging
# to an instance
# \param soddatatype SOD data type to use
# \param objectindex String with the SOD index
# \param subindex String with the SOD subindex
# \return String with identifier for an actual data / variable definition
def get_actval_identifier(instance_str, soddatatype, objectindex, subindex):
    identifier = ''.join([
        SODDataType_ActData_Prefix.get_DataWord_string(soddatatype), '_',
        instance_str, '_act_', get_identifier_index_string(objectindex),
        '_', get_identifier_subindex_string(subindex)
        ])
    return identifier

##
# \}
# \}
