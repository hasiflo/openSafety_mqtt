##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file configparser.py
# Implements parser for reading settings files (pysodbsettings.ini)
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
import sys

# third party packages or modules

# own packages or modules
from pysodb.core import common
from pysodb.core import errorhandler

# check python version because of different naming of needed configparser
if sys.version_info.major == 2:
    import ConfigParser as configparser
    configparser.ConfigParser.read_file = configparser.ConfigParser.readfp
elif sys.version_info.major >= 3:
    import configparser as configparser
else:
    errorhandler.ErrorHandler.error(
                'Used Python Version is not supported.')  #coverage#: no cover

mandatory_settingsfile_sections_keys = {
    'EPLSCfg' : {
                 'maxInstances', 'maxPayloadLength',
                 'errorString', 'errorStatistic',
                 'scm', 'applicationObjects', 'saplReportStateChange'
                 },
    'Constants' : {'configString'},
    'SPDOCfg' : {
                 'frameCopyIntern', 'numLookupTable',
                 'connectionValidBitField',
                 'connectionValidStatistic', 'maxRxSyncedPerTx',
                 'notAnsweredTr', 'propDelayStatistic', 'spdo40bitCTSupport',
                 'spdoExtendedCTBitField'
                 },
    'TXSPDOCom' : {
                   'maxMapEntries', 'maxPayloadSize',
                   'spdosActivatedPerCall', 'maxSpdo',
                   'maxSpdoSdg'
                   },
    'RXSPDOCom' : {
                   'maxMapEntries', 'maxPayloadSize',
                   'spdosActivatedPerCall', 'maxSpdo',
                   'maxSpdoSdg'
                   },
    'SCMCfg' : {
                'maxNodes', 'processedNodesPerCall',
                'maxSadr'
                }
    }


##
# \brief Class to parse the settings file
class SODBuilderConfigParser():
    def __init__(self):
        self.parser = configparser.ConfigParser()
        self.parser.optionxform = str

    ##
    # \brief Reads in the settings file
    #
    # \param fileobj File object obtained from open()
    def read_configfile(self, fileobj):
        try:
            self.parser.read_file(fileobj)
        except Exception as e:
            errorhandler.ErrorHandler.error(
                            'Error reading settingsfile: {}'.format(e))

    ##
    # \brief Prints the read in data which is stored in the cofig parser
    def print_config(self):
        for section in self.parser.sections():
            errorhandler.ErrorHandler.debug('section:{}'.format(section))
            for option in self.parser.options(section):
                errorhandler.ErrorHandler.debug(
                    '--{0}:{1}'.format(option, self.parser.get(section, option))
                    )
    ##
    # \brief Checks the read in data (which is stored in the cofig parser)
    # for the mandatory / expected sections and attributes
    def _check_mandatory_items(self):
        sections_of_read_file = self.parser.sections()
        mandatory_sections = mandatory_settingsfile_sections_keys.keys()
        missing_sections = list(set(mandatory_sections) -
                                set (sections_of_read_file))

        if len(missing_sections) > 0:
            errorhandler.ErrorHandler.error(
                'Missing mandatory sections in settingsfile: {}'.format(
                    ', '.join(missing_sections))
                )

        for section in mandatory_settingsfile_sections_keys.keys():
            if not section in missing_sections:
                keys_of_section_from_file = self.parser.options(section)
                mandatory_keys_of_section = \
                    mandatory_settingsfile_sections_keys[section]
                missing_keys = list (set (mandatory_keys_of_section) -
                                     set (keys_of_section_from_file))

                if len (missing_keys) > 0:
                    errorhandler.ErrorHandler.error(
                        'Missing mandatory keys for section {}: {}'.format(
                            section, ', '.join(missing_keys))
                        )

    ##
    # \brief  Populates and returns the EPLScfg datastructure from parsed data
    #
    # \return EPLScfg datastructure
    def get_settings(self):

        self._check_mandatory_items()

        eplscfg = common.EPLSCfg()
        constants = eplscfg.constants
        spdocfg = eplscfg.spdoCfg
        txspdocom = eplscfg.spdoCfg.tx
        rxspdocom = eplscfg.spdoCfg.rx
        scmcfg = eplscfg.scmCfg

        for section in self.parser.sections():
            if (section == 'EPLSCfg'):
                for option in self.parser.options(section):
                    setattr(eplscfg, option, self.parser.get(section, option))

            elif (section == 'Constants'):
                for option in self.parser.options(section):
                    setattr(constants, option, self.parser.get(section, option))

            elif (section == 'SPDOCfg'):
                for option in self.parser.options(section):
                    setattr(spdocfg, option, self.parser.get(section, option))

            elif (section == 'TXSPDOCom'):
                for option in self.parser.options(section):
                    setattr(txspdocom, option, self.parser.get(section, option))

            elif (section == 'RXSPDOCom'):
                for option in self.parser.options(section):
                    setattr(rxspdocom, option, self.parser.get(section, option))

            elif (section == 'SCMCfg'):
                for option in self.parser.options(section):
                    setattr(scmcfg, option, self.parser.get(section, option))

        return eplscfg

##
# \}
# \}
