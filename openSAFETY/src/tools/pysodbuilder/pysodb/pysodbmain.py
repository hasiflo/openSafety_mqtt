##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# Python helper tool to customize and generate the files EPLScfg.h,
# sod.h and sod.c based on cmake settings and an osdd file.
# \{
# \details
# \{
# The pysodbuilder tool allows the customisation and generation of the files
# EPLSCfg.h, sod.h and sod.c based on settings made in cmake and on
# data for a given osdd file. It's main intent is to ease and accelerate the
# customisation of the application specific parts of the openSAFTEY stack and
# the openSAFTEY demo, respectively. It primarily reads in the source files
# which contain special markup for the code generation and processes them into
# the output files with respect to the settings and osdd information.
# Also, it is able to generate a binding file based on the used osdd XML schema
# to enable the processing of osdd files for the main use case.
#
# The tool relies on third party packages for osdd file processing,
# as well as the code generation, which must be installed before using
# pysodbuilder:
# - cogapp 2.4: see https://pypi.python.org/pypi/cogapp/2.4
# - PyXB 1.2.4: see https://pypi.python.org/pypi/PyXB/
#
# \author Alexander Brunner, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
# \}
# \file pysodbmain.py
# Main file of pysodbuillder

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
from pysodb.core import argparser
from pysodb.core import errorhandler
from pysodb.core import util
from pysodb.core import filehandling

##
# \brief main function
#
# This function represents the main program.
# It gets called from /scripts/pysodbuilder or from within this file,
# when pysodbmain is executed directly.
def main():

    # setup console output logging to verbose, before reading in
    # commandline arguments with verbose setting,
    # because of possible error output during argument validation
    consolehandle = errorhandler.ErrorHandler.add_console_output(
        errorhandler.get_level(True))

    # read in commandline args
    parser = argparser.init_argparser()
    args = argparser.parse_args(parser)
    args = argparser.validate_args(parser, args)

    # set errorhandler / logging according commandline argument
    consolehandle.setLevel(errorhandler.get_level(args.verbose))

    # set logging to file according commandline argument
    if args.log_file:
        errorhandler.ErrorHandler.add_file_output(
            errorhandler.get_level(args.verbose), args.log_file)

    errorhandler.ErrorHandler.debug(args)

    # set encoding
    if args.file_encoding:
        filehandling.FileHandling.set_encoding(args.file_encoding)

    # generate binding file
    if args.generate_binding_file:
        util.generate_binding_file (args.generate_binding_file[0],
                                    args.generate_binding_file[1])

    # read in xosdd File and populate data structure (binding class) with data
    osdd_data = None
    if args.osdd_file:
        osdd_data = util.get_osdd_data_from_osddfile(args.osdd_file)

    if args.list_modules:
        util.list_osdd_modules(osdd_data)
        sys.exit(common.SODBsysexit.EXIT_SUCCESS)

    if osdd_data:
        osdd_module = util.get_osdd_module(osdd_data, args.module_id)

    # read in settings file and populate data structure with data
    if args.settings_file:
        sodb_settings = util.get_settings_from_settingsfile(args.settings_file)

    if not (args.input_files_dirs):
        sys.exit(common.SODBsysexit.EXIT_SUCCESS)

    # start cog with given parameters and generate needed code
    if args.dry_run:
        # print inputfiles with generated code to stdout
        util.generate_files_with_cog(args.input_files_dirs, None,
                                     common.DefaultFileExtensions.EXT_COG,
                                     sodb_settings, osdd_module, None,
                                     args.remove_cog_comments,
                                     args.overrule_osdd, args.file_encoding)
    else:
        # do the real code generation
        util.generate_files_with_cog(args.input_files_dirs,
                                     args.output_directory,
                                     common.DefaultFileExtensions.EXT_COG,
                                     sodb_settings, osdd_module, args.overwrite,
                                     args.remove_cog_comments,
                                     args.overrule_osdd, args.file_encoding)

    sys.exit(common.SODBsysexit.EXIT_SUCCESS)

if __name__ == '__main__':
    main()

##
# \}
# \}
