##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file gentestc.py
# A simplyfied CodeGenUtil class used for unit tests

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
from pysodb.codegeneration.codegenutil import CodeGenUtil, generatorfunc

##
# \brief Class for generating a C test file. It is used for unittests, testing
# the basic code generation functionality
class GenTestc(CodeGenUtil):

    ##
    # \brief object initialisation function
    def __init__(self, codegenutildata, language):
        super(GenTestc, self).__init__(codegenutildata, language)
        self.outputdefname = 'OUTSTR'

    ##
    # \brief genrates include directive
    @generatorfunc
    def generate_includes(self):
        value = '#include <stdio.h>'
        self.codegen.gen_rawCode(value)

    ##
    # \brief Generates a multiline comment
    @generatorfunc
    def generate_defines_section(self):
        value = '"Constants / Defines"'
        self.codegen.gen_comment_sectionheader(value)

    ##
    # \brief Generates a define
    @generatorfunc
    def generate_defines(self):
        value = '"Hello World!"'
        self.codegen.gen_definition(self.outputdefname, value)

    ##
    # \brief generates a printf statement
    @generatorfunc
    def generate_printf(self):
        value = 'printf("%s", {});'.format(self.outputdefname)
        self.codegen.gen_rawCode(value)


def init():
    from pysodb.codegeneration.codegenutil import cgendata
    cg = GenTestc(cgendata, 'C')
    return cg

##
# \}
# \}
# \}
