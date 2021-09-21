##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file gensodutilh.py
# Functionality to generate sodutil.h
#
# This file acts as interface for the code code generation and encapsulates
# all functionality directly needed to generate parts of the file sod.h.

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
from pysodb.codegeneration.c_code_util import generate_structure, \
    CBitAccessMacroAdapter
from pysodb.codegeneration.codegenutil import generatorfunc, CodeGenUtil
from pysodb.util.sod_data import SODDefines
from pysodb.util.stringfunctions import get_hex_string_leading_0x
from pysodb.osddprocessing.processingdata import OSDD_SOD_ProcessingData


##
# \brief This class links the input source files with the code generation logic
#
# The GenEPLScfgh class acts as link between the input source files with their
# specific markup and the logic for generation the desired code.
# It encapsulates all functions, which can be called from outside i.e.
# the input source file.
class Gensodutilh(CodeGenUtil):

    ##
    # \brief Initialisation function
    # \param codegenutildata Instance of CodeGenUtilData
    # \param language Language of the code which should get generated
    def __init__(self, codegenutildata, language):
        super(Gensodutilh, self).__init__(codegenutildata, language)

        self.processingdata = OSDD_SOD_ProcessingData(
                                    self.osdd_module, self.sodb_settings)
    ##
    # \brief Generates the define USEDCHANNELSIZE, the total count of SPDO
    # channels
    @generatorfunc
    def usedchannelsize(self):
        value = get_hex_string_leading_0x(self.processingdata.used_channels, 2)
        comment = self.codegen.get_comment_inline_doxygen_member(
                                        'The total count of SPDO channels')
        self.codegen.gen_definition_with_comment(
                                    'USEDCHANNELSIZE', value, comment)

    ##
    # \brief Generates structue definitions for parameterstream settings
    # and settings with SOD positions
    @generatorfunc
    def settings_structs(self):

        structlist = self.processingdata.settings_structs_paramstream \
            + [self.processingdata.settingsgroups_struct_paramstream,
               self.processingdata.settings_sodpos_struct] \
            + self.processingdata.spdo_data_structs

        num_structs = len (structlist)

        for struct in structlist:

            if struct is None:
                continue

            generate_structure(self.codegen, struct)

            if num_structs > 1:
                self.codegen.gen_rawCode('')

            num_structs -= 1

    ##
    # \brief Generates statements referencing the used variables as extern
    @generatorfunc
    def extern_struct_instances(self):
        for struct in self.processingdata.spdo_data_structs:
            code = ' '.join(['extern', struct.datatype, struct.identifier,
                             SODDefines.SAFE_NO_INIT_SEKTOR])
            code = code + ';'
            self.codegen.gen_rawCode(code)

    ##
    # \brief Generates macros for accessing (read, write) a bit in a SPDO
    # variable
    @generatorfunc
    def bit_access_macros(self):

        num_macros = len(self.processingdata.bit_access_macros)
        if num_macros > 0:
            self.codegen.gen_comment_sectionheader(
                                    'macros for bit access to SPDO data')

        for macro in self.processingdata.bit_access_macros:
            macroadapter = CBitAccessMacroAdapter(macro)
            code = macroadapter.get_read_bit_macro()
            self.codegen.gen_rawCode(code)
            code = macroadapter.get_write_bit_macro()
            self.codegen.gen_rawCode(code)

            if num_macros > 1:
                self.codegen.gen_rawCode('')
                num_macros -= 1


##
# \brief Intialisation function of the module.
#
# When this module is used by an input source file,
# this function creates an instance of the needed CodeGenUtil sublcass
# which then can be used for code generation
def init():
    from pysodb.codegeneration.codegenutil import cgendata
    cg = Gensodutilh(cgendata, 'C')
    return cg

##
# \}
# \}
