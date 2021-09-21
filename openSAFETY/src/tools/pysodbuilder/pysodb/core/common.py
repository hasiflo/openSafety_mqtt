##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file core/common.py
# Encapsulates several constants and data structures used by multiple modules
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


##
# \brief Holds information about the pysodbuilder program
class SODBinfo():
    PROGRAM_NAME = 'pysodbuilder'
    LOGGER_NAME = 'pysodblogger'


##
# \brief Constants / Exitcodes for program exit
class SODBsysexit():
    EXIT_SUCCESS = 0
    EXIT_FAILURE = 1


##
# \brief Constants for default file extensions
class DefaultFileExtensions():
    EXT_BINDING = '.py'
    EXT_SCHEMA = '.xsd'
    EXT_OSDD = '.xosdd'
    EXT_SETTINGS = '.ini'
    EXT_COG = ['.c', '.cpp', '.h']


##
# \brief Stores SCMCfg settings read from settings file
class SCMCfg(object):
    def __init__(self):
        self.maxNodes = 0
        self.processedNodesPerCall = 0
        self.maxSadr = 0


##
# \brief Stores SPDOcom settings read from settings file
class SPDOCom(object):
    def __init__(self):
        self.maxMapEntries = 0
        self.maxPayloadSize = 0
        self.spdosActivatedPerCall = 0
        self.maxSpdo = 0
        self.maxSpdoSdg = 0


##
# \brief Stores SPDOCfg settings read from settings file
class SPDOCfg(object):
    def __init__(self):
        self.frameCopyIntern = False
        self.numLookupTable = False
        self.connectionValidBitField = False
        self.connectionValidStatistic = False
        self.maxRxSyncedPerTx = 0
        self.notAnsweredTr = 0
        self.propDelayStatistic = False
        self.spdo40bitCTSupport = False
        self.spdoExtendedCTBitField = False
        self.tx = SPDOCom()
        self.rx = SPDOCom()


##
# \brief Stores Constants settings read from settings file
class Constants(object):
    def __init__(self):
        self.enable = 1
        self.disable = 0
        self.notApplicable = 2000
        self.configString = ''


##
# \brief Stores EPLSCfg settings read from settings file
class EPLSCfg(object):
    def __init__(self):
        self.maxInstances = 0
        self.maxPayloadLength = 0

        self.errorString = False
        self.errorStatistic = False
        self.scm = False
        self.applicationObjects = False
        self.saplReportStateChange = False

        self.scmCfg = SCMCfg()
        self.spdoCfg = SPDOCfg()
        self.constants = Constants()

##
# \}
# \}
