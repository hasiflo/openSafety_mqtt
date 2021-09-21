##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file util/structure.py
# Abstract data model for a structure / record
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
from pysodb.codegeneration.c_target_data import TargetDataTypes


##
# \brief Represents a stucture member for Structue objects
class Structmember(object):
    def __init__(self, datatype, identifier,
                 comment, initval=None, arraysize=None):
        self.datatype = datatype
        self.identifier = identifier
        self.comment = comment
        self.initval = initval
        self.arraysize = arraysize


##
# \brief Represents a structure data type
class Struct (object):
    def __init__(self, datatype, memberlist=[], identifier=None, comment=None):
        self.datatype = datatype

        #use memberlist, if one was passed,
        #if the list was not passed or is empty,
        #make a copy, to guarantee differrent lists
        #between two instances of SPDOStruct
        if len(memberlist) > 0:
            self.members = memberlist
        else:
            self.members = memberlist[:]

        self.identifier = identifier
        self.comment = comment
        self.initval = []

        self.refresh_initvalues()

    ##
    # \brief Refreshes initial values of the struct members
    def refresh_initvalues(self):
        self.initval = []

        for item in self.members:
            self.initval.append(item.initval)


##
# \brief Represents a data structure for SPDO data
class SPDOStruct(Struct):
    def __init__(self, transportname, memberlist=[],
                 identifier=None, comment=None):

        self.datatype = ''.join(['tSPDOTransport', transportname])

        #use memberlist, if one was passed,
        #if the list was not passed or is empty,
        #make a copy, to guarantee differrent lists
        #between two instances of SPDOStruct
        if len(memberlist) > 0:
            self.members = memberlist
        else:
            self.members = memberlist[:]

        if identifier:
            self.identifier = identifier
        else:
            self.identifier = ''.join(['transp', transportname, '_g'])

        self.comment = comment


##
# \brief Represents a SOD_t_ACT_LEN_PTR_DATA data structure used in C source
# code
class SOD_ActLenPtrData(Struct):
    def __init__(self, ptr_data=None, act_len=None,
                  identifier=None, sektor=None):
        datatype = 'SOD_t_ACT_LEN_PTR_DATA'
        self.identifier = identifier
        self.sektor = sektor
        self.act_len = Structmember(TargetDataTypes.UINT32, 'dw_actLen',
                                     None, act_len)
        self.ptr_data = Structmember(TargetDataTypes.VOID, 'pv_objData',
                                      None, ptr_data)
        members = [self.act_len, self.ptr_data]

        super(SOD_ActLenPtrData, self).__init__(datatype, members, identifier)

##
# \}
# \}
