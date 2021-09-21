##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file util/macro.py
# Classes which represent a macro
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
from pysodb.util.constants import NumberSystemBases


##
# \brief Class for accessing single bits of bigger data types
#
# This class represents a macro which enables accessing single bits
# of bigger data types. It can be used to generate code for bit accesses of
# for the target language.
class BitAccessMacro(object):

    ##
    # \brief Initialises a BitAccessMacro object
    #
    # \param macroname Name or identifier of the macro
    # \param variablename Name or identifier of  the variable to use
    # \param bitmask Bit mask of the bit to access
    # \param cast_to_datatype Data type to which the bit should be casted
    # on read or write
    def __init__(self, macroname, variablename, bitmask, cast_to_datatype=None):
        self.macroname = macroname
        self.variablename = variablename
        self.bitmask = bitmask
        self.cast_to_datatype = cast_to_datatype
        self.index = 0

        #check for int, assume a stringtype, if not
        if not isinstance(bitmask, int):
            self.bitmask = int(bitmask, NumberSystemBases.HEX)
            bitmask = self.bitmask

        #calculate index from bitmask
        while True:
            bitmask = bitmask >> 1
            if bitmask == 0:
                break
            self.index = self.index + 1

##
# \}
# \}
