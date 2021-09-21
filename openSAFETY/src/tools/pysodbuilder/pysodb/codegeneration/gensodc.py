##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file gensodc.py
# Functionality to generate sod.c
#
# This file acts as interface for the code code generation and encapsulates
# all functionality directly needed to generate parts of the file sod.c.

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
from pysodb.codegeneration.c_code_util import to_c_sod_entry
from pysodb.codegeneration.c_target_data import TargetDataTypes
from pysodb.codegeneration.codegenutil import CodeGenUtil, generatorfunc
from pysodb.util.sod_data import SODDefines, SODDataTypes, \
    SODEntryNumEntries, SODVariable
from pysodb.util.structure import SOD_ActLenPtrData
from pysodb.util.type_conversion import SODDataType_SODRangeDataType, \
    SODDataType_TargetDataType
from pysodb.core.errorhandler import ErrorHandler
from pysodb.osddprocessing.processingdata import OSDD_SOD_ProcessingData


##
# \brief This class links the input source files with the code generation logic
#
# The Gensodc class acts as link between the input source files with their
# specific markup and the logic for generation the desired code.
# It encapsulates all functions, which can be called from outside i.e.
# the input source file.
class Gensodc(CodeGenUtil):

    ##
    # \brief Initialisation function
    # \param codegenutildata Instance of CodeGenUtilData
    # \param language Language of the code which should get generated
    def __init__(self, codegenutildata, language):
        super(Gensodc, self).__init__(codegenutildata, language)

        self.processingdata = OSDD_SOD_ProcessingData(self.osdd_module,
                                                      self.sodb_settings)

    ##
    # \brief Generates range variable definitions
    @generatorfunc
    def range_definitions(self):
        rangedict = self.processingdata.rangedict

        for datatype in SODDataType_SODRangeDataType.get_SODRangeDataTypes():

            ranges = [rangedict[obj] for obj in rangedict \
                      if rangedict[obj].datatype == datatype]
            sorted_ranges = sorted(ranges, key=lambda x: x.identifier)

            for arange in sorted_ranges:

                low = arange.low
                high = arange.high
                identifier = arange.identifier

                code = ''.join(['static ' , 't_', datatype, '_RANGE ',
                                identifier, ' = ', '{', str(low), ', ',
                                str(high), '};'])
                self.codegen.gen_rawCode(code)

    ##
    # \brief Generates the variable definitions used for specifying the number
    # of following SOD enties
    @generatorfunc
    def number_of_entries_definitions(self):
        for noe in self.processingdata.numberofentries:
            comment = self.codegen.get_comment_inline_doxygen_member(
                ''.join (['number of entries = ', str(noe)]))
            code = ''.join(['static ', TargetDataTypes.UINT8, ' b_noE_',
                            str(noe), ' ', SODDefines.SAFE_INIT_SEKTOR, ' = ',
                            str(noe), ';'])
            code = ''.join([code, self.codegen.getTab(1, len(code)), comment])
            self.codegen.gen_rawCode(code)

    ##
    # \brief Generates the variable definition used for the structure  of
    # parameter stream settings
    @generatorfunc
    def paramstream_settings_definition(self):
        if not self.processingdata.settingsgroups_struct_paramstream:
            return

        initstring = str(
            self.processingdata.settingsgroups_struct_paramstream.initval)

        translation_dict = {'[':'{', ']':'}', 'None':'0', '\'':''}

        for key in translation_dict:
            initstring = initstring.replace(key, translation_dict[key])

        righthand_expression = ''.join([initstring, ';'])
        code = ' '.join([
            'static',
            self.processingdata.settingsgroups_struct_paramstream.datatype,
            self.processingdata.settingsgroups_struct_paramstream.identifier,
            '=', righthand_expression])
        self.codegen.gen_rawCode(code)

    ##
    # \brief Generates the variable definition used for the structure  of
    # settings with SOD position
    @generatorfunc
    def sodpos_settings_definition(self):

        if not self.processingdata.settings_sodpos_struct:
            return

        self.processingdata.settings_sodpos_struct.refresh_initvalues()

        initstring = str(self.processingdata.settings_sodpos_struct.initval)

        translation_dict = {'[':'{', ']':'}', 'None':'0', '\'':''}

        for key in translation_dict:
            initstring = initstring.replace(key, translation_dict[key])

        righthand_expression = ''.join([initstring, ';'])
        code = ' '.join([
            'static', self.processingdata.settings_sodpos_struct.datatype,
            self.processingdata.settings_sodpos_struct.identifier, '=',
            righthand_expression])
        self.codegen.gen_rawCode(code)

    ##
    # \brief Generates the SOD entries
    @generatorfunc
    def make_sod_entries(self):
        index = 0
        oldindex = 0

        sodentries = sorted(self.processingdata.sodentries)
        if len(sodentries) == 0:
            return

        for sodentry in sodentries[:-1]:
            index = sodentry.objectindex
            if index != oldindex:
                self.codegen.gen_rawCode('')
                oldindex = index
            if sodentry.comment:
                self.codegen.gen_comment(sodentry.comment)
            self.codegen.gen_rawCode(to_c_sod_entry(sodentry))

        end_entry = sodentries[-1]
        if end_entry.comment:
            self.codegen.gen_rawCode('')
            self.codegen.gen_comment(end_entry.comment)
        self.codegen.gen_rawCode(to_c_sod_entry(end_entry, True))

    ##
    # \brief Generates the variable definitions used for default values
    @generatorfunc
    def default_values_definitions(self):
        sodentries = sorted(self.processingdata.sodentries)

        variables = []
        comment = None
        index = 0

        for entry in sodentries:
            if (entry.subindex == 0):
                # save comment for later, to print at variable definitions
                # with every new SOD index
                comment = entry.comment
                index = entry.objectindex

            if not isinstance(entry, SODEntryNumEntries):
                if isinstance(entry.objectdataref , SOD_ActLenPtrData):
                    if isinstance(entry.objectdataref.ptr_data.initval,
                                  SODVariable):

                        if comment and entry.objectindex == index:
                            variables.append(
                                (entry, entry.objectdataref.ptr_data.initval,
                                 comment))
                            comment = None
                        else:
                            variables.append(
                                (entry, entry.objectdataref.ptr_data.initval,
                                 None))

                if entry.attributes.defaultval_ref \
                and not entry.attributes.defaultval_ref.used_for_generation:
                    entry.attributes.defaultval_ref.used_for_generation = True
                    if comment and entry.objectindex == index:
                        variables.append(
                            (entry, entry.attributes.defaultval_ref, comment))
                        comment = None
                    else:
                        variables.append(
                            (entry, entry.attributes.defaultval_ref, None))

        #generate code
        comment = None
        for entry, variable, comment in variables:

            if comment:
                self.codegen.gen_rawCode('')
                self.codegen.gen_comment(comment)

            datatype = None

            if not variable.datatype:
                if variable.arraysize and entry.attributes.datatype \
                in [SODDataTypes.DOM, SODDataTypes.OCT]:
                    datatype = TargetDataTypes.UINT8
                else:
                    datatype = SODDataType_TargetDataType.get_TargetDataType(
                                                    entry.attributes.datatype)
            else:
                datatype = variable.datatype

            if variable.sektor:
                sektor = ' ' + variable.sektor
            else:
                sektor = ''

            if variable.arraysize:
                arraysize = ''.join(['[', variable.arraysize, ']'])
            else:
                arraysize = ''

            if variable.value:
                expression = ''.join([' = ', variable.value])
            else:
                expression = ''

            code = ''.join(['static ', datatype, ' ',
                            variable.identifier, arraysize,
                            sektor, expression, ';' ])
            self.codegen.gen_rawCode(code)

    ##
    # \brief Generates the variable definitions for actual data /
    # structs of target type SOD_t_ACT_LEN_PTR_DATA
    @generatorfunc
    def act_data_definitions(self):
        act_data = self.processingdata.actvalue_structs

        for item in act_data:
            datatype = item.datatype
            identifier = item.identifier

            if item.sektor:
                sektor = ' ' + item.sektor
            else:
                sektor = ''

            if len(item.initval) != 2:
                ErrorHandler.error('structure SOD_ActLenPtrData does not have '\
                                   'a valid number of members')

            if isinstance(item.initval[1], SODVariable):
                if item.initval[1].arraysize:
                    element_zero = '[0]'
                else:
                    element_zero = ''
                pointerdatastr = ''.join(['&', item.initval[1].identifier,
                                          element_zero])
            else:
                pointerdatastr = ''.join(['&', item.initval[1]])
            structmemberstr = ', '.join([item.initval[0], pointerdatastr])

            initstring = ''.join(['{', structmemberstr, '}'])

            code = ''.join(['static ', datatype, ' ', identifier, sektor,
                            ' = ', initstring, ';' ])
            self.codegen.gen_rawCode(code)


##
# \brief Intialisation function of the module.
#
# When this module is used by an input source file,
# this function creates an instance of the needed CodeGenUtil sublcass
# which then can be used for code generation
def init():
    from pysodb.codegeneration.codegenutil import cgendata
    cg = Gensodc(cgendata, 'C')
    return cg

##
# \}
# \}
