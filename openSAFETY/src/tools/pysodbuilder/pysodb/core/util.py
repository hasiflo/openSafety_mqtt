##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file util.py
# Splits the functionality of the main program into several smaller functions
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
import os

# third party packages or modules
import cogapp
import pyxb.utils.domutils as domutils

# own packages or modules
from pysodb.codegeneration import codegenutil
from pysodb.core import configparser
from pysodb.core import errorhandler
from pysodb.core import filehandling


##
# \brief Enables checking, if a given output directory is in the list of input
# files or directories
#
# This function checks, if the output directory name / path is in the list of
# input files or directories.
#
# \param files_directories_list List with file and directory names / paths
# \param output_directory Directory name / path
#
# \return True, when output_directory is in list of input files / directories;
# False otherwise
def check_outpdirectory_in_input_files_directories(files_directories_list,
                                                   output_directory):
    if os.path.isdir(output_directory):
        for item in files_directories_list:
            if os.path.exists(item):
                if os.path.isfile(item):
                    input_directory = os.path.dirname(os.path.abspath(item))
                else:
                    input_directory = item
                #if os.path.samefile(input_directory, output_directory):
                if (os.stat(input_directory) == os.stat(output_directory)) and \
                (os.path.abspath(input_directory) == \
                 os.path.abspath(output_directory)):
                    return True
    return False


##
# \brief Wrapping function for os.path.isfile()
#
# Checks, if a file with given file name exists
#
# \param filename File name to check for existence
#
# \returns True, if file exists; False otherwise
def check_file_exists(filename):
    return os.path.isfile(filename)


##
# \brief Checks, if a given directory name exists
#
# \param directory Directory name to check for existence
#
# \returns True, if directory exists; False otherwise
def check_directory_exists(directory):
    if os.path.exists(directory):
        if os.path.isdir(directory):
            return True
    return False


##
# \brief Gets those files of specified files and in specified folders
# which have the given extensions
#
# \param directory_file_list List with file names and directories
# \param extensions_list List with file extensions, which should be recognised
# for the given files and searched for such files in the given directories
#
# \return List with found files matching the extensions
# \return List with files and directories which do not exist on the system.
# but were given in directory_file_list
def get_files_matching_extensions(directory_file_list, extensions_list):
    result = []
    not_found = []

    if not directory_file_list or not extensions_list:
        return result, not_found

    for dir_file in directory_file_list:
        if os.path.exists(dir_file):
            if os.path.isdir(dir_file):
                for file in os.listdir(dir_file):
                    if os.path.isfile(os.path.join(dir_file, file)) and \
                    file.endswith(tuple(extensions_list)):
                        result.append(os.path.join(dir_file, file))
            elif dir_file.endswith(tuple(extensions_list)):
                result.append(dir_file)
        else:
            not_found.append(dir_file)

    result = list(map(os.path.normpath, result))

    return result, not_found


##
# \brief Generates arguments for cog
#
# This function generates the command line like arguments for use with the
# cogapp third party module
#
# \param inputfile File name of the source file to process
# \param outputfile File name of the output file
# \param overwrite Flag to enable (True) or disable (False) overwriting
# the input source files
# \param remove_cog_comments Flag to enable (True) or disable (Fals) the
# removal of the special code generation markup from the source input files
# in the output files
# \param encoding File encoding to use for processing the files
# \param verbosity Verbosity level for cogapp code generator
#
# \return Arguments for cogapp
def make_cog_arguments(inputfile, outputfile=None, overwrite=True,
                       remove_cog_comments=False, encoding=None, verbosity=0,):
    cogargs = []
    cogargs.append('cog')
    if outputfile:
        cogargs.append('-o')
        cogargs.append(outputfile)
    elif overwrite:
        cogargs.append('-r')
    # else cog will print generated code

    if verbosity is not None:
        cogargs.append('--verbosity={}'.format(verbosity))

    if remove_cog_comments:
        cogargs.append('-d')

    if encoding is not None:
        cogargs.append('-n')
        cogargs.append(encoding)

    if inputfile is not None:
        cogargs.append(inputfile)

    return cogargs


##
# \brief Translates a path with (back) slashes to package format (dots)
#
# \param path Path to translate
# \return Path in package format
def translate_relative_path_to_package(path):
    path = os.path.normpath(path)
    path = path.replace('\\', '/')
    path = os.path.normpath(path)
    path = path.replace('/', '.')
    path = path.replace('\\', '.')
    return path


##
# \brief Lists osdd module IDs for given osdd_data
#
# \param osdd_data Data structure (obtained from get_osdd_data_from_osddfile())
def list_osdd_modules(osdd_data):
    errorhandler.ErrorHandler.info(
        'the specified osdd file has following modules:')
    if osdd_data:
        for module in osdd_data.Module:
            errorhandler.ErrorHandler.info(module.id)


##
# \brief Gets a specific osdd_module from osdd_data, needed when an osdd file
# contains several modules
#
# \param osdd_data Data structure (obtained from get_osdd_data_from_osddfile())
# \param module_id String for specifying the desired module
#
# \return osdd Module with the specified module ID
def get_osdd_module(osdd_data, module_id):
    module_position = 0

    if osdd_data is None:
        errorhandler.ErrorHandler.error('osdd_data is None')

    if module_id:
        for module in osdd_data.Module:
            if module.id == module_id:
                return osdd_data.Module[module_position]
            module_position = module_position + 1
        errorhandler.ErrorHandler.error('osdd file doesn\'t have module with '\
                                        'module id "{}"'.format(module_id))
    else:
        if len(osdd_data.Module) > 1:
            errorhandler.ErrorHandler.error(
                'osdd file contains several modules, but none of '\
                'them was selected')
        else:
            return osdd_data.Module[0]


##
# \brief Reads in an osdd file and populates the datastructure defined
# by the used (and generatable) binding file
#
# \param osdd_filename File name of the osdd file to read
# \param module Module with binding code for accessing XML data
# \return osdd data structure defined by used binding file
def get_osdd_data_from_osddfile(osdd_filename,
                                module='pysodb.binding.xosddbinding'):
    import importlib
    try:
        xosdd = importlib.import_module(module)
    except Exception as e:
        errorhandler.ErrorHandler.error('bindingfile not found: {}'.format(e))

    with filehandling.FileHandling.open(osdd_filename, mode='r') as xml_file:
        xml = xml_file.read()

    try:
        osdd_dom = domutils.StringToDOM(xml)
    except:
        errorhandler.ErrorHandler.error('error reading xml file to DOM')

    try:
        osdd_data = xosdd.CreateFromDOM(osdd_dom.documentElement)
    except:
        errorhandler.ErrorHandler.error(
            'error populating data structure from DOM')

    return osdd_data


##
# \brief Reads settings from settings file and returns data structure
#
# This function opens the given settings file, parses the content and
# returns the settings data as object of core.common.EPLSCfg class
#
# \param settings_file File name of the settings file
#
# \return settings data as object of core.common.EPLSCfg class
def get_settings_from_settingsfile(settings_file):
    with filehandling.FileHandling.open(settings_file, 'r') as settings_file:
        cfgparser = configparser.SODBuilderConfigParser()
        cfgparser.read_configfile(settings_file)

    sodb_settings = cfgparser.get_settings()
    return sodb_settings


##
# \brief Starts code generation with the help of the cogapp module
#
# This function uses the cog code generator to read in input files and
# generate desired code.
#
# \param include_files_dirs Input files and directories to search for files for
# processing
# \param output_directory Directory for the output files
# \param extensions_list File extensions used for file search
# \param sodb_settings Settings from settings file
# \param osdd_module OSDD module data to use and calculate values from
# \param overwrite Enable (True), or disable (False) the overwriting  of the
# source input files
# \param remove_cog_comments Enable (True), or disable (False) the removal of
# the specific code generation markup code in the output files
# \param overrule_osdd Enable (True), or disavle (False) the overruling of from
# osdd calculated values, if the values in the settings file are bigger
# \param encoding Encoding to use for the file processing
def generate_files_with_cog(
        include_files_dirs, output_directory, extensions_list, sodb_settings,
        osdd_module, overwrite, remove_cog_comments, overrule_osdd,
        encoding=None):

    outfilename = None
    errorhandler.ErrorHandler.debug(
        'Calling cog, using module id "{}"'.format(osdd_module.id))

    filenames, not_found = get_files_matching_extensions(include_files_dirs,
                                                         extensions_list)
    if not_found:
        errorhandler.ErrorHandler.error('could not find files / directories: '\
                                        '{}'.format(' ,'.join(not_found)))

    for filename in filenames:

        errorhandler.ErrorHandler.debug('')
        errorhandler.ErrorHandler.debug('Processing file "{}"'.format(filename))

        if output_directory:
            outfilename = os.path.normpath(
                '/'.join([output_directory, os.path.basename(filename)]))
            create_directory_if_not_exists(output_directory)
            errorhandler.ErrorHandler.debug(
                'writing to file "{}"'.format(outfilename))
        else:
            errorhandler.ErrorHandler.debug(
                'writing to file "{}"'.format(filename))

        cogargs = make_cog_arguments(filename, outfilename, overwrite,
                                     remove_cog_comments, encoding)

        outfuncs = []
        if output_directory or overwrite:
            outfuncs = [errorhandler.ErrorHandler.debug]


        # read sourcefiles / start cog, which calls generator functions
        cog = cogapp.Cog()

        codegenutil.CodeGenUtilData(outfuncs, cog.cogmodule, sodb_settings,
                                    osdd_module, filename, overrule_osdd)

        cog.callableMain(cogargs)


##
# \brief Creates given directory, if it does not exist
#
# \param directory Name of the directory to create
def create_directory_if_not_exists(directory):

    abs_directory = os.path.abspath(directory)

    if not check_directory_exists(abs_directory):
        try:
            os.makedirs(abs_directory)
        except:
            errorhandler.ErrorHandler.error(
                'could not create directory {}'.format(directory))


##
# \brief Generates a binding file with given schema
#
# This function generates a binding file for a given osdd schema file.
# Whenever the schema for the used osdd file changes, it is mandatory to
# generate a new binding file.
#
# \param schema_filename File name of the XML schema file which the used
# osdd files adhere to
# \param output_filename File name of the output file
def generate_binding_file(schema_filename, output_filename):
    import pyxb.binding.generate

    if schema_filename is None or output_filename is None:
        errorhandler.ErrorHandler.error('cannot generate binding file, '\
                                        'schema or output file not specified!')

    output_directory = os.path.dirname(os.path.abspath(output_filename))
    create_directory_if_not_exists(output_directory)

    bindingcode = pyxb.binding.generate.GeneratePython(
        schema_location=schema_filename)

    with filehandling.FileHandling.open(output_filename, 'w') as file:
        file.write(bindingcode)

##
# \}
# \}
