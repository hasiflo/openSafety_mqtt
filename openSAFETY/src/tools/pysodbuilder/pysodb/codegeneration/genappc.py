##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file genappc.py
# Functionality to generate app.c
#
# This file acts as interface for the code code generation and encapsulates
# all functionality directly needed to generate parts of the file app.c.

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
from pysodb.codegeneration.codegenutil import CodeGenUtil, generatorfunc
from pysodb.util.sod_data import SODDefines
from pysodb.osddprocessing.processingdata import OSDD_SOD_ProcessingData


##
# \brief This class links the input source files with the code generation logic
#
# The Genappc class acts as link between the input source files with their
# specific markup and the logic for generation the desired code.
# It encapsulates all functions, which can be called from outside i.e.
# the input source file.
class Genappc(CodeGenUtil):

    ##
    # \brief Initialisation function
    # \param codegenutildata Instance of CodeGenUtilData
    # \param language Language of the code which should get generated
    def __init__(self, codegenutildata, language):
        super(Genappc, self).__init__(codegenutildata, language)
        self.processingdata = OSDD_SOD_ProcessingData(self.osdd_module,
                                                      self.sodb_settings)
        self.defines = []

    ##
    # \brief Generates the structure variable declarations for SPDO data
    @generatorfunc
    def spdo_struct_declaration(self):
        for struct in self.processingdata.spdo_data_structs:
            code = ' '.join([struct.datatype, struct.identifier,
                             SODDefines.SAFE_NO_INIT_SEKTOR])
            code = code + ';'
            comment = self.codegen.get_comment_inline_doxygen_member(
                                                                struct.comment)
            code = ''.join([code, self.codegen.getTab(1, len(code)), comment])
            self.codegen.gen_rawCode(code)

    ##
    # \brief Generates the initalisation code of the SPDO data
    #
    # The SPDO data variables will be assigned the value 0.
    @generatorfunc
    def app_reset(self):
        num_structs = len (self.processingdata.spdo_data_structs)
        counter = 0

        for struct in self.processingdata.spdo_data_structs:

            if counter < num_structs:
                self.codegen.gen_rawCode('')
                counter += 1

            for structmember in struct.members:
                code = ''.join([struct.identifier, '.', structmember.identifier,
                                ' = 0;'])
                self.codegen.gen_rawCode(code)

    ##
    # \brief Generaes the APP_SPDO_NUM defines
    @generatorfunc
    def app_spdo_num_defines(self):
        self.defines = []
        max_number_spdos = max(len(self.processingdata.spdotransport_input),
                               len(self.processingdata.spdotransport_output))

        comment = 'Id of the SPDO of the application'
        comment = self.codegen.get_comment_inline_doxygen_member(comment)

        for i in range(max_number_spdos):
            identifier = ''.join(['APP_SPDO_NUM', str(i)])
            self.defines.append(identifier)
            value = ''.join(['(', TargetDataTypes.UINT16, ')', str(i)])
            self.codegen.gen_definition_with_comment(identifier, value, comment)

    ##
    # \brief Generaes the connection valid checks according to
    # generated APP_SPDO_NUM defines
    @generatorfunc
    def connection_valid_checks(self):
        funccode = 'pfnGetConValid_l('
        funccalls = []

        for identifier in self.defines:
            funccalls.append(''.join([funccode, identifier, ')']))

        expression = ' && '.join(funccalls)

        code = ''.join(['if(', expression, ')'])
        self.codegen.gen_rawCode(code)


##
# \brief Intialisation function of the module.
#
# When this module is used by an input source file,
# this function creates an instance of the needed CodeGenUtil sublcass
# which then can be used for code generation
def init():
    from pysodb.codegeneration.codegenutil import cgendata
    cg = Genappc(cgendata, 'C')
    return cg

##
# \}
# \}
