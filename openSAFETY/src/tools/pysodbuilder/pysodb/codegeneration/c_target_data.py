##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file c_target_data.py
# Constants for C language data type definitions based on target.h

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
# \brief Groups data types used in sod.c, sod.h or sodutil.h
#
# This class groups (previously defined) data types used in C code based on the
# file target.h
class TargetDataTypes(object):
    CHAR = 'CHAR'
    INT8 = 'INT8'
    UINT8 = 'UINT8'
    BOOL = 'BOOLEAN'
    INT16 = 'INT16'
    UINT16 = 'UINT16'
    INT = 'INT'
    INT32 = 'INT32'
    UINT = 'UINT'
    UINT32 = 'UINT32'
    INT64 = 'INT64'
    UINT64 = 'UINT64'
    NULL = 'NULL'
    VOID = 'void'

##
# \}
# \}
