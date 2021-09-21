##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file codegenutil.py
# Utility functions for code generation
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
from datetime import datetime

# third party packages or modules

# own packages or modules
from pysodb.core import codegenerator
from pysodb.core import versioninfo
from pysodb.core.common import SODBinfo


##
# \brief Borg class, working similar like a Singleton pattern.
#
# Allows creating multiple instances of the class
# but all instances share  the same state.
class Borg(object):
    __state = {}

    def __new__(cls, *p, **k):
        self = object.__new__(cls)
        self.__dict__ = cls.__state
        return self


##
# \brief Subclass of Borg for sharing data
#
# This class is used for communication between the main part of
# the SODBuilder and particular code generation modules
class CodeGenUtilData(Borg):

    ##
    # \brief Initialisation function
    #
    # \param outfuncs List of function names which will be called, when
    # generated code should be placed somewhere (e.g. write to file, show on
    # command line)
    # \param cogmodule Code generation module of the third party cogapp package
    # \param sodb_settings Data structure populated from pysodbsettings.ini file
    # (see \ref core.common.EPLSCfg)
    # \param osdd_module Module object obtained from osdd file
    # \param filename Name of the current processed input file
    # \param overrule_osdd Status flag which states whether the values obtained
    # from osdd file can be overruled by the settings made in cmake or not
    def __init__(self, outfuncs=[], cogmodule=None, sodb_settings=None,
                 osdd_module=None, filename=None, overrule_osdd=None):
        self.outfuncs = outfuncs
        self.cogmodule = cogmodule
        self.sodb_settings = sodb_settings
        self.osdd_module = osdd_module
        self.filename = filename
        self.overrule_osdd = overrule_osdd

        global cgendata

        cgendata = self


cgendata = CodeGenUtilData()


##
# \brief Decorator for generator functions
#
# This decorator initializes the Codegenerators cog output function
# before the wanted function gets called
# and deinitializes the cog output function after
# the function returned.
# This is needed, because cog's outfunction references
# aren't the same between several generator function calls.
def generatorfunc(func):

        def func_wrapper(inst, *args, **kwargs):
            inst._geninit()
            retval = func(inst, *args, **kwargs)
            inst._gendeinit()
            return retval
        return func_wrapper


##
# \brief Code generation utility class
#
# Stores refernces to objects used for codegeneration.
# sublcass this class to write generatorfunctions.
class CodeGenUtil(object):

    language = ''

    def __new__(cls, *p, **k):
        self = object.__new__(cls)
        return self

    ##
    # \brief Initialisation function
    # \param codegenutildata Instance of CodeGenUtilData
    # \param language Language of the code which should get generated
    def __init__(self, codegenutildata, language):
        self.codegenutildata = codegenutildata
        self.language = language

        self.init()

        self.generationtime = datetime.now()
        self._get_code_generator(language)

    ##
    # \brief Initialising the class members dependent from a CodeGenUtilData
    # object
    #
    # This function initialises the class members which are dependent from
    # a CodeGenUtilData object. If a subclass of CodeGenUtil is used only
    # by one input source file (is used only once), this function is called
    # from the class initialisation function.
    # If a subclass is used multiple times in the same program run,
    # this function has to be called from the input source file to refresh
    # the data that possibly changed in the CodeGenUtilData object
    def init(self):
        self.cogmodule = self.codegenutildata.cogmodule
        self.outfuncs = self.codegenutildata.outfuncs
        self.osdd_module = self.codegenutildata.osdd_module
        self.filename = self.codegenutildata.filename
        self.sodb_settings = self.codegenutildata.sodb_settings

    ##
    # \brief initialize codegenerator output function
    #
    # initializes the output function of the codegenerator
    # with the cog.outl function
    def _geninit(self):
        if self.cogmodule is not None:
            self.codegen.addoutfunc(self.cogmodule.outl)

    ##
    # \brief removes cog.outl function from codegenerator output functions
    def _gendeinit(self):
        if self.cogmodule is not None:
            self.codegen.removeoutfunc(self.cogmodule.outl)

    ##
    # \brief Get codegenerator instance of specified language
    #
    # Sets the member codegenerator to a codegenerator
    # instance for the language supplied in \_\_init\_\_()
    def _get_code_generator(self, language):
        cgenclass = codegenerator.get_codegenerator_class(language)
        if cgenclass:
            self.codegen = cgenclass()
            for outfunc in self.outfuncs:
                self.codegen.addoutfunc(outfunc)
        else:
            raise TypeError ('Did not get an appropriate CodeGenerator.')

    ##
    # \brief Generates an infostring (program name, version, date)
    # in the output file
    @generatorfunc
    def generator_info(self):
        time = datetime.now()
        self.codegen.gen_comment(
            ' '.join([SODBinfo.PROGRAM_NAME ,
            versioninfo.SODBVersionInfo.openSAFETY_VERSION_RELEASE,
            'generated file for', self.sodb_settings.constants.configString,
            '| generated on:', time.strftime('%Y-%m-%d %H:%M:%S')])
        )

##
# \}
# \}
