##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file osddprocessing/postprocessing.py
# Implements functionality for post processing an OSDD module.
# After post processing, code can be generated.
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
from pysodb.util.constants import IdentifierPlaceholder
from pysodb.util.sod_data import SODEntryNumEntries, SODDataTypes, \
    SODVariable, SODStandardObjects
from pysodb.util.stringfunctions import get_defval_identifier, \
    get_actval_identifier, get_struct_range_identifier, \
    get_struct_actual_data_identifier
from pysodb.util.structure import SOD_ActLenPtrData
from pysodb.util.type_conversion import SODDataType_SODRangeDataType


##
# \brief Does the post processing like updating default values,
# variable declarations and calculating the number of SOD entries and
# value ranges.
# After post processing, code can be generated.
class PostProcessing(OSDDProcessingBase):

    ##
    # \brief Initialises a PostProcessing object
    #
    # \param processingdata_obj OSDD_SOD_ProcessingData object
    # \param instance Instance number, the identifiers with instance
    # placeholders will be replaced with the given value
    def __init__(self, processingdata_obj, instance=None):
        super(PostProcessing, self).__init__(processingdata_obj)
        self.instance = instance

    ##
    # \brief Processes variables, default values, SOD value ranges and
    # actual value references / reference names
    def do_postprocessing(self):

        self.processingdata.numberofentries = self._process_SODEntryNumEntries()

        if self.instance is not None:
            instance_str = str(self.instance)
        else:
            instance_str = IdentifierPlaceholder.INSTANCE

        for entry in self.processingdata.sodentries:
            self._process_default_value(entry, instance_str,
                                        self.processingdata.variables)
            self._process_range(entry, self.processingdata.rangedict)
            self._process_actual_value_variable_declaration(
                    entry, instance_str, self.processingdata.variables)
            self._process_actual_value_instance(
                    entry, instance_str, self.processingdata.actvalue_structs)

    ##
    # \brief Resolves the number of SOD entries of SODEntryNumEntries objects
    #
    # This method resolves / calculates the number of SOD entries at the,
    # following subindexes of an object index when an SODEntryNumEntries is
    # found.
    def _process_SODEntryNumEntries(self):

        sod_entries = [entry for entry in self.processingdata.sodentries \
                       if isinstance(entry, SODEntryNumEntries)]
        numberofentries = []

        for entry in sod_entries:
            itemsatindex = [item for item in self.processingdata.sodentries \
                            if item.objectindex == entry.objectindex \
                            and item.subindex != entry.subindex ]
            num = len(itemsatindex)
            numberofentries.append(num)
            entry.set_objectdata_and_defaultval_references_noE(num)

        return sorted(set(numberofentries))

    ##
    # \brief Processes the SODDefaultValue objects, set datatypes,
    # (array sizes), and identifier / reference to the default value
    #
    # \param sodentry SODEntry object to process
    # \param instance_str Instance string which will replace the instance
    # placeholder in an identifier
    # \param defvallist List with SODDevaultValue objects to which
    # a possible default value of an SODEntry will be added
    def _process_default_value(self, sodentry, instance_str, defvallist):

        if sodentry.attributes.defaultval_ref:
            defval = sodentry.attributes.defaultval_ref

            if defval.process_later:
                defval.process_later = False

                if sodentry.attributes.datatype in [SODDataTypes.OCT,
                SODDataTypes.DOM]:

                    if sodentry.attributes.datatype == SODDataTypes.OCT:
                        if sodentry.attributes.max_length:
                            defval.arraysize = sodentry.attributes.max_length

                defval.identifier = get_defval_identifier(
                    instance_str, sodentry.attributes.datatype,
                    sodentry.objectindex, sodentry.subindex)

            defvallist.append(defval)

    ##
    # \brief Processes the SODVariable objects which can be used in a SOD entry
    #
    # \param sodentry SODEntry object to process
    # \param instance_str Instance string which will replace the instance
    # placeholder in an identifier
    # \param variable_declarations List to which a possible SOD_ActLenPtrData
    # object will be added, if such a variable declaration or definition
    # is needed
    def _process_actual_value_variable_declaration(self, sodentry, instance_str,
                                                   variable_declarations):
        if isinstance(sodentry.objectdataref, SOD_ActLenPtrData):
            actlen_ptrdata = sodentry.objectdataref
            if isinstance(actlen_ptrdata.ptr_data.initval, SODVariable):
                ptr_data = actlen_ptrdata.ptr_data.initval

                if ptr_data.process_later:
                    ptr_data.process_later = False

                    if sodentry.attributes.datatype in [SODDataTypes.DOM,
                                                        SODDataTypes.OCT]:
                        ptr_data.arraysize = sodentry.attributes.max_length

                    if not ptr_data.identifier:
                        ptr_data.identifier = get_actval_identifier(
                            instance_str, sodentry.attributes.datatype,
                            sodentry.objectindex, sodentry.subindex)
                    else:
                        identifier = ptr_data.identifier.replace(
                                IdentifierPlaceholder.INSTANCE, instance_str)
                        ptr_data.identifier = identifier

                    variable_declarations.append(actlen_ptrdata)

    ##
    # \brief Processes the SODRange objects, sets the datatypes, and
    # the identifier / reference to the range
    #
    # \param sodentry SODEntry object to process
    # \param rangedict Dictionary with SODRanges to which a possible range
    # of the SODEntry will be added
    def _process_range (self, sodentry, rangedict):
        if sodentry.rangeref:
            arange = sodentry.rangeref
            arange.datatype = SODDataType_SODRangeDataType.get_SODRangeDataType(
                sodentry.attributes.datatype)
            arange.identifier = get_struct_range_identifier(
                arange.datatype, arange.low, arange.high)

            rangedict[arange.identifier] = arange

    ##
    # \brief Processes the identifiers / references to actual object
    # data
    #
    # \param sodentry SODEntry object to process
    # \param instance_str Instance string which will replace the instance
    # placeholder in an identifier
    # \param structlist List with structures to append the SOD_ActLenPtrData
    # object of a SODEntry object to
    def _process_actual_value_instance(self, sodentry,
                                       instance_str, structlist):
        if sodentry.objectdataref:
            if isinstance(sodentry.objectdataref, SOD_ActLenPtrData):

                actlenptrdata = sodentry.objectdataref

                #set correct instance
                initval = actlenptrdata.ptr_data.initval
                if isinstance(initval, str):
                    actlenptrdata.ptr_data.initval = initval.replace(
                                IdentifierPlaceholder.INSTANCE, instance_str)

                if not actlenptrdata.act_len.initval:
                    actlenptrdata.act_len.initval = \
                        sodentry.attributes.max_length

                actlenptrdata.refresh_initvalues()

                if not actlenptrdata.identifier:
                    actlenptrdata.identifier = \
                        get_struct_actual_data_identifier(
                        instance_str, sodentry.objectindex, sodentry.subindex)
                    structlist.append(actlenptrdata)

            else:
                #objectdataref is a string
                sodentry.objectdataref = sodentry.objectdataref.replace(
                    IdentifierPlaceholder.INSTANCE, instance_str)

                offset = None

                # process offsets of SPDO positions (offsets) if they are in
                # the index range of communicationparameters
                rxstart = SODStandardObjects.RxSPDOCommunicationParameter.start
                rxend = SODStandardObjects.RxSPDOCommunicationParameter.end

                txstart = SODStandardObjects.TxSPDOCommunicationParameter.start
                txend = SODStandardObjects.TxSPDOCommunicationParameter.end

                if sodentry.objectindex >= rxstart \
                and sodentry.objectindex <= rxend:
                    offset = sodentry.objectindex - rxstart

                elif sodentry.objectindex >= txstart \
                and sodentry.objectindex <= txend:
                    offset = sodentry.objectindex - txstart

                if offset is not None:
                    sodentry.objectdataref = sodentry.objectdataref.replace(
                        IdentifierPlaceholder.SPDO_POSITION, str(offset))

##
# \}
# \}
