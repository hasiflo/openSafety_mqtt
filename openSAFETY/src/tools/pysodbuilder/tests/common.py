##
# addtogroup unittest
# \{
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file common.py
# Classes and functions used as stubs for unit testing, unit test related code

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

current_python_version = sys.version_info[0:2]

## Python version constant for version checking in unittest fixtures
python34 = (3, 4)

## Python version constant for version checking in unittest fixtures
python32 = (3, 2)

## Python version constant for version checking in unittest fixtures
python27 = (2, 7)

## Attributes which a DataStore object must have
expected_datastore_entries = [
        'epls_cfg_max_pyld_len',
        'epls_k_configuration_string',

        'spdo_cfg_max_no_tx_spdo',
        'spdo_cfg_max_tx_spdo_mapp_entries',
        'spdo_cfg_max_len_of_tx_spdo',

        'spdo_cfg_max_no_rx_spdo',
        'spdo_cfg_max_rx_spdo_mapp_entries',
        'spdo_cfg_max_len_of_rx_spdo',
        ]

def compare_files_ignore_EOL(file1, file2):
    linef1 = ' '
    linef2 = ' '
    with open(file1, 'r') as f1, open(file2, 'r') as f2:
        while linef1 != '' and linef2 != '':
            linef1 = f1.readline()
            linef2 = f2.readline()
            if linef1 != linef2:
                return False
    return True

def check_expected_attributes(testcase, object_to_test, expected_attributes):
    for item in expected_attributes.items():
        key = item[0]
        value = item[1]

        testcase.assertTrue(hasattr(object_to_test, key))
        attr = getattr(object_to_test, key)
        testcase.assertEqual(attr, value)


# ###### functions for testing / patching of the original functions
##
# \brief Function for patching getepls_enable_disable_from_string
def dummy_getepls_enable_disable_from_string(value):
    return value


##
# \brief Function for patching e.g. codegenerator output functions
def print_wrapper(text):
    print (text)


##
# \brief Function for patching e.g. codegenerator output functions
def debug_print(*args, **kwargs):
    print (args, kwargs)


##
# \brief Function for patching others to test behaviour when an Exception is
# raised.
def dummy_func_raise_exception(*args, **kwargs):
    raise Exception


##
# \brief General function for patching
def dummy_func_pass(*args, **kwargs):
    pass


##
# \brief Function for patching generatorfunc() decorator
def dummy_generatorfunc_decorator(func):
    def func_wrapper (inst, *args, **kwargs):
        return func(inst, *args, **kwargs)
    return func_wrapper


##
# \brief Function for patching classes __init__()
def class_init_dummy (self, *args, **kwargs):
    pass


# ###### data structure for testing / faking settings
##
# \brief Data structure for testing / faking a SPDOCom object used
# for handling settings data
class DummySPDOCom(object):
    def __init__(self):
        self.maxMapEntries = 0
        self.maxPayloadSize = 0
        self.maxSpdo = 0


##
# \brief Data structure for testing / faking a SPDOCfg object used
# for handling settings data
class DummySPDOCfg(object):
    def __init__(self):
        self.tx = DummySPDOCom()
        self.rx = DummySPDOCom()


##
# \brief Data structure for testing / faking a Constants object used
# for handling settings data
class DummyConstants(object):
    def __init__(self):
        self.configString = 'test'


##
# \brief Data structure for testing / faking a SODBsettings object
class DummySODBsettings(object):
    def __init__(self):
        self.maxPayloadLength = 0
        self.constants = DummyConstants()
        self.spdoCfg = DummySPDOCfg()


# ###### data structure for testing / faking osdd_modules
##
# \brief Data structure for testing / faking an AcyclicTransport object
# (data from an osdd file read into data structure provided by xosddbinding.py)
class DummyAcyclicTransport(object):
    def __init__(self):
        self.OctetSize = 0


##
# \brief Data structure for testing / faking a SPDOTransport object
# (data from an osdd file read into data structure provided by xosddbinding.py)
class DummySPDOTransport(object):
    def __init__(self, name=None, direction=None, octetsize=0):
        self.Name = name
        self.maxNrMappings = 0
        self.Direction = direction
        self.OctetSize = octetsize


##
# \brief Data structure for testing / faking a DataTransport object
# (data from an osdd file read into data structure provided by xosddbinding.py)
class DummyChannelConfigurationDataTransport(object):
    def __init__(self):
        self.SPDOTransport = [DummySPDOTransport(), DummySPDOTransport()]


##
# \brief Data structure for testing / faking a ChannelConfiguration object
# (data from an osdd file read into data structure provided by xosddbinding.py)
class DummyChannelConfiguration(object):
    def __init__(self):
        self.DataTransport = DummyChannelConfigurationDataTransport()


##
# \brief Data structure for testing / faking a DataTransport object
# (data from an osdd file read into data structure provided by xosddbinding.py)
class DummyModuleDefinitionDataTransport(object):
    def __init__(self):
        self.ACyclicTransport = [DummyAcyclicTransport(),
                                 DummyAcyclicTransport()]


##
# \brief Data structure for testing / faking a ModuleDefinition object
# (data from an osdd file read into data structure provided by xosddbinding.py)
class DummyModuleDefinition(object):
    def __init__(self):
        self.DataTransport = DummyModuleDefinitionDataTransport()


##
# \brief Data structure for testing / faking an OSDD module object
# (data from an osdd file read into data structure provided by xosddbinding.py)
class DummyOSDDModule(object):
    def __init__(self):
        self.ChannelConfiguration = DummyChannelConfiguration()
        self.ModuleDefinition = DummyModuleDefinition()
        self.id = None


# ###### data structure for testing / faking settings / osdd_modules
##
# \brief Data structure for testing / faking an OSDD module
class DummyDataStore(object):
    def __init__(self, attributelist, seed=0):
        if attributelist is None or isinstance(attributelist,
                (DummySODBsettings, DummyOSDDModule)):
            attributelist = expected_datastore_entries
        for item in attributelist:
            setattr(self, item, seed)
            seed += 1


##
# \brief Data structure for testing / faking a DataChooser
class DummyDataChooser(object):

    def __init__(self, *args, **kwargs):
        self.returnvalue = 0
        self.calledwith = None

    def get(self, *args, **kwargs):
        self.calledwith = (args, kwargs)
        return self.returnvalue

    def get_calledwith(self):
        return self.calledwith


##
# \brief Data structure for testing / faking a Cog module / object
class DummyCogModule(object):
    def outl(self, text):
        print (text)


##
# \brief Data structure for testing / faking a CodeGenUtilData object
class DummyCodeGenUtilData(object):
    def __init__(self):
        self.cogmodule = DummyCogModule()
        self.outfuncs = [print_wrapper]
        self.osdd_module = DummyOSDDModule()
        self.filename = 'file.name'
        self.sodb_settings = DummySODBsettings()
        self.overrule_osdd = False


##
# \brief Class for catching generated output
class OutputCatcher(object):
    caught_text = []
    lines = 0

    @classmethod
    def out(cls, text):
        cls.caught_text.append(text)
        cls.lines += 1

    @classmethod
    def read(cls):
        returnval = cls.caught_text
        cls.caught_text = []
        cls.lines = 0
        return returnval


##
# \brief Data structure for testing / faking a parsed argument data structure
class DummyArgs(object):
    pass


##
# \brief Class for faking objects
#
# This class adds dynamically attributes and methods to itself, when
# an (so far not existing) attribute is assigned a value or a method is called.
# This allows to check the frequency of specific function / method calls and
# the particular function call parameters.
# Before an object of the class is used in the tested code, it is possible to
# specify returnvalues for functions which are known to get called and values of
# attributes.
class MagicDummy(object):

    ##
    # \brief Object initialisation method
    def __init__(self, *args, **kwargs):
        self._func_calls_parameter = []
        self._last_func_parameters = None
        self.returnvalue = None
        self.raiseerror = None
        self.added_attributes = []

    ##
    # \brief Deletes dynamically added attributes
    def _delete_added_attributes(self):
        for attribute in self.added_attributes:
            delattr(self, attribute)

    ##
    # \brief Resets the object to the initial state
    def reset(self):
        self._delete_added_attributes()
        self.__init__()

    ##
    # \brief Returns the number of calls the object / function / method
    # has been called
    def get_number_of_calls(self):
        return len(self._func_calls_parameter)

    ##
    # \brief Returns the parameters of the last called function / method
    def get_last_function_call_params(self):
        params = self._last_func_parameters
        self._last_func_parameters = None
        return params

    ##
    # \brief Returns all parameters of all calls of the function / method
    def get_function_call_params(self):
        return self._func_calls_parameter

    ##
    # \brief Function that saves the parameters of the function call
    #
    # This function saves the parameters of the function call and returns
    # a return value, which was specified earlier in time, or raises an
    # previously defined error.
    #
    # \param args Positional arguments
    # \param kwargs Keyword arguments
    # \return Previously specified return value or raises an prevoiusly
    # specified error
    def dummy_func(self, *args, **kwargs):
        self._func_calls_parameter.append((args, kwargs))
        self._last_func_parameters = (args, kwargs)
        if self.raiseerror:
            raise self.raiseerror
        return self.returnvalue

    ##
    # \brief This function adds the called functio, or the attribute to the
    # object, if it is not part of the object.
    #
    # __getattr__() gets called from the Python interpreter, if it cannot find
    # the desired attribute or function / mehtod.
    # In such a case this function adds dynamically the specified function,
    # if code outside tries to call a function, which is not already part of
    # the object.
    # The same behaviour applies to attribute assignments.
    # Nested access to attributes or functions is also possible.
    #
    # \param name Name of the desired attribute / function / method
    # \return New instance of the MagicDummy class
    def __getattr__(self, name):
        if name not in self.__dict__.keys():

            newdummy = MagicDummy()
            setattr(self, name, newdummy)
            setattr(self, 'name', name)
            self.added_attributes.append(name)

            return newdummy

    ##
    # \brief Makes the object callable, i.e. the object can imitate an attribute
    # as well as a function
    def __call__(self, *args, **kwargs):
        return self.dummy_func(*args, **kwargs)


##
# \brief For faking a CodeGen
class DummyCodeGen(MagicDummy):
    pass


##
# \brief For faking an argparser
class DummyArgparser(MagicDummy):
    pass


##
# \brief For faking an ErrorHandler
class DummyErrorHandler(MagicDummy):

    def raise_SystemExit(self, *args, **kwargs):
        self._last_func_parameters = (args, kwargs)
        raise SystemExit

    def dummy_func(self, *args, **kwargs):
        self._last_func_parameters = (args, kwargs)
        return self.returnvalue

    def __getattr__(self, name):
        if name not in self.__dict__.keys():
            self._last_func_called = name

            if name == 'error' or name == 'exception':
                return self.raise_SystemExit
            return self.dummy_func


##
# \brief For imitating an OSDDSetting object
class DummyOSDDSetting(MagicDummy):

    def __init__(self, aid, name, template, atype, dataselectiontype,
                 value, unit, visibility, paramstreampos, sodpos, *args, **kwargs):
        super(DummyOSDDSetting, self).__init__(*args, **kwargs)
        self.id = aid
        self.Name = name
        self.Template = template
        self.Type = atype
        self.Value = value
        self.Unit = unit
        self.Visibility = visibility
        self.ParameterStream = None
        self.DataSelection = None
        if dataselectiontype:
            self.DataSelection = MagicDummy()
            self.DataSelection.Type = dataselectiontype
        if paramstreampos:
            self.ParameterStream = MagicDummy()
            self.ParameterStream.Position = paramstreampos
        self.SODPosition = None
        if sodpos:
            self.SODPosition = MagicDummy()
            self.SODPosition.Index = sodpos[0]
            self.SODPosition.SubIndex = sodpos[1]


##
# \brief For imitating an SODEntry object
class DummySODEntry(MagicDummy):
    def __init__(self, objectindex, subindex):
        self.objectindex = objectindex
        self.subindex = subindex


##
# \brief For imitating an SODEntry object,
# without the behaviour of a MagicDummy class
class DummySimpleSODEntry(object):
    def __init__(self):
        self.objectindex = 0

##
# \}
# \}
# \}
