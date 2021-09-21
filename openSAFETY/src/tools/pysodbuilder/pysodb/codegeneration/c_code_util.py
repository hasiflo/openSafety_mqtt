##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file codegeneration/c_code_util.py
# Functionality for translating pysodbuilder object data to C code
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
from pysodb.codegeneration.c_target_data import TargetDataTypes
from pysodb.util.constants import CodeFormatConstants
from pysodb.util.macro import BitAccessMacro
from pysodb.util.sod_data import SODDataTypes
from pysodb.util.stringfunctions import get_sod_index_string, \
    get_sod_subindex_string, get_hex_string_leading_0x
from pysodb.util.structure import SOD_ActLenPtrData
from pysodb.util.type_conversion import SODDataTypes_ObjectLength_CLang
from pysodb.core.errorhandler import ErrorHandler


##
# \brief Converts a SODEntry object to an SOD entry in C code
#
# \param sodentry SODEntry object
# \param islastentry Flag for marking the last entry of the SOD, because for
# this, other code is needed
# \return SOD entry as C language string
def to_c_sod_entry(sodentry, islastentry=False):

    defvalrefstr = None
    defvalref = sodentry.attributes.defaultval_ref

    if defvalref is None:
        defvalrefstr = TargetDataTypes.NULL
    else:
        defvalrefstr = defvalref.identifier

    if defvalrefstr != TargetDataTypes.NULL and defvalrefstr != '0':
        if sodentry.attributes.datatype != SODDataTypes.OCT:
            defvalrefstr = ''.join(['&', defvalref.identifier])
        #else no '&' operator is prepended

    objectdataref = sodentry.objectdataref
    if objectdataref is None:
        objectdataref = TargetDataTypes.NULL

    elif isinstance(objectdataref, SOD_ActLenPtrData):
        if objectdataref.identifier:
            objectdataref = objectdataref.identifier
            objectdataref = ''.join(['&', objectdataref])
        else:
            ErrorHandler.error('SOD_ActLenPtrData object does not '\
                               'have an identifier!')

    elif objectdataref != TargetDataTypes.NULL and defvalrefstr != '0':
        objectdataref = ''.join(['&', objectdataref])

    rangeref = sodentry.rangeref
    if rangeref is None:
        rangeref = TargetDataTypes.NULL

    if rangeref != TargetDataTypes.NULL  and defvalrefstr != '0':
        rangeref = ''.join(['&', rangeref.identifier])

    attrlist = ' | '.join(sodentry.attributes.attributes)
    max_length = sodentry.attributes.max_length

    if max_length is None:
        max_length = SODDataTypes_ObjectLength_CLang.get_ObjectLength(
                                                sodentry.attributes.datatype)

    sodattr = ', '.join([attrlist, sodentry.attributes.datatype,
                         max_length, defvalrefstr])
    inner_sod = ''.join(['{', sodattr, '}'])

    sodindex = None
    sodsubindex = None

    if sodentry.indexalias:
        sodindex = sodentry.indexalias
    else:
        sodindex = get_sod_index_string(sodentry.objectindex)

    if sodentry.subindexalias:
        sodsubindex = sodentry.subindexalias
    else:
        sodsubindex = get_sod_subindex_string(sodentry.subindex)

    callback = sodentry.callback
    if callback is None:
        callback = TargetDataTypes.NULL

    result = ', '.join([ sodindex, sodsubindex, inner_sod,
                        objectdataref, rangeref, callback])

    if not islastentry:
        return ''.join(['{', result, '},'])
    else:
        return ''.join(['{', result, '}'])


##
# \brief Generates C structure code from the given Structure object with the
# given code generator
#
# \param codegen code generator to use
# \param struct Structure object from wich the code will be generated
def generate_structure(codegen, struct):

    if struct.comment:
        codegen.gen_comment_multiline_doxygen([ ' '.join ([r'\brief',
                                                           struct.comment])])
    codegen.gen_rawCode('typedef struct')
    codegen.gen_rawCode('{')

    for member in struct.members:
        comment = ''
        if member.comment:
            comment = codegen.get_comment_inline_doxygen_member(member.comment)
        code = ''.join([member.datatype, ' ', member.identifier, ';'])

        if comment:
            code = ''.join([code, codegen.getTab(1, len(code)), comment])

        codegen.gen_rawCode(''.join([codegen.getTab(), code]))

    codegen.gen_rawCode(''.join(['} ', struct.datatype, ';']))


##
# \brief Class for translating a BitAccessMacro object to
# C language macros
#
# This class enables getting C language macros for reading and writing
# particular bits of a variable out of a BitAccessMacro object
class CBitAccessMacroAdapter(object):

    ##
    # \brief Initialises the class
    #
    # \param macro BitAccessMacro object, to use for the generation of
    # C language code
    def __init__(self, macro):
        if isinstance(macro, BitAccessMacro):
            self.macro = macro
        else:
            raise TypeError('Expected parameter of type BitAccessMacro')

    ##
    # \brief Returns a C code macro for reading a bit
    def get_read_bit_macro(self):
        macroname = ''.join(['READ_', self.macro.macroname])
        macrocontent = '(('

        if self.macro.cast_to_datatype:
            macrocontent = ''.join(['(', macrocontent,
                                    '(', self.macro.cast_to_datatype, ') (',
                                    self.macro.variablename, '))'])
        else:

            macrocontent = ''.join([macrocontent, self.macro.variablename])

        macrocontent = ''.join([macrocontent, ' >> ',
            str(self.macro.index), ') & 0x01)' ])

        macro = ' '.join(['#define', macroname, macrocontent])
        return macro

    ##
    # \brief Returns a C code macro for writing a bit
    def get_write_bit_macro(self):
        macroname = ''.join(['WRITE_', self.macro.macroname, '(VAL)'])
        macrocontent = ''.join([
            '((', self.macro.variablename, ' = (', self.macro.variablename,
            ' & (~', get_hex_string_leading_0x(
                self.macro.bitmask, CodeFormatConstants.BITMASK_WIDTH),
            ')) | ((VAL & 0x01) << ',
            str(self.macro.index), ')))'])

        macro = ' '.join(['#define', macroname, macrocontent])
        return macro

##
# \}
# \}

