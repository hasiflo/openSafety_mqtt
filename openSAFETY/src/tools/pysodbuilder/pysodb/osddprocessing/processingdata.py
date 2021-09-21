##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file osddprocessing/processingdata.py
# Converts data from an OSDD module to a format usable by code generation
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
from pysodb.osddprocessing.mainprocessing import MainProcessing
from pysodb.osddprocessing.postprocessing import PostProcessing
from pysodb.osddprocessing.preprocessing import PreProcessing

from pysodb.util.sod_data import SODEntriesList


##
# \brief Class which converts data of a OSDD module to Objects used for
# code generation
class OSDD_SOD_ProcessingData(object):

    ##
    # \brief Initialises the object and calls all the processing functions
    # used for transforming OSDD information to data whihch can be used for
    # code generation
    #
    # \param osdd_module OSDD module data obtained via the binding file
    # \param sodb_settings settings data from settings file
    def __init__(self, osdd_module, sodb_settings):

        self.sodb_settings = sodb_settings

        self.numberofentries = set()

        #preprocessing, reading data, transform it to a suitable form
        #for further processing
        self._channelconfiguration = osdd_module.ChannelConfiguration
        self._designconfiguration = osdd_module.DesignConfiguration
        self._modulefunctions = osdd_module.ModuleFunctions

        self.spdotransport_output = []
        self.spdotransport_input = []
        self.settingdatatemplates = []
        self.channelgroups = []
        self.functions = []
        self.settingsgroups = []
        self.used_channels = 0

        self.preprocessing = PreProcessing(self)
        self.preprocessing.do_preprocessing()

        #generate SOD entries and structures needed for code generation
        self.sodentries = SODEntriesList()

        #main processing
        self.spdo_data_structs = []
        self.bit_access_macros = []

        self.settings_structs_paramstream = []
        self.settingsgroups_struct_paramstream = None

        self.settings_sodpos_struct = None

        self.mainprocessing = MainProcessing(self)
        self.mainprocessing.do_mainprocessing()

        # postprocessing of SOD entries
        self.rangedict = {}
        self.actvalue_structs = []
        self.variables = []

        self.postprocessing = PostProcessing(self, 0)
        self.postprocessing.do_postprocessing()

##
# \}
# \}
