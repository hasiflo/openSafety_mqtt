##
# \addtogroup Tools
# \{
# \addtogroup pysodbuilder openSAFETY pysodbuilder
# \{
#
# \file util/defaultsod.py
# Default SOD definition with predefined SOD entries
#
# This file contains the SODDefaultEntries class, which encapsulates
# the default SOD with predefined SOD entries, wich are used by
# OSDD_SOD_Converter to build a custom SOD

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
import copy

# third party packages or modules

# own packages or modules
from pysodb.util.sod_data import SODEntryEnd, SODEntriesList, SODEntry, \
    SODAttr, SODAttributes, SODDataTypes, SODCallback, SODIndexComments, \
    SODEntryNumEntries, SODDefaultValue, SODDefines, SODRange, SODVariable
from pysodb.util.structure import SOD_ActLenPtrData


##
# \brief Keeps default SOD entries
class SODDefaultEntries(object):

    __sod_entry_end = SODEntryEnd()

    __sod_default_entries = SODEntriesList([

        #Error register
        SODEntry(0x1001, 0x00, SODAttr([SODAttributes.RO], SODDataTypes.U8, None, None), 's_?INSTANCE?_act_general.errRegister.ucErrorRegister', None, SODCallback.SOD_k_NO_CALLBACK, SODIndexComments.ErrorRegister),

        #Error statistics
        SODEntryNumEntries(0x1004, 0x00, SODIndexComments.ErrorStatistics),
        SODEntry(0x1004, 0x01, SODAttr([SODAttributes.RO], SODDataTypes.U32, None, None), 'SERR_aadwCommonEvtCtr[?INSTANCE?][SERR_k_SFS_LENGTH]', None, SODCallback.SOD_k_NO_CALLBACK),
        SODEntry(0x1004, 0x02, SODAttr([SODAttributes.RO], SODDataTypes.U32, None, None), 'SERR_aadwCommonEvtCtr[?INSTANCE?][SERR_k_SFS_TOO_LONG]', None, SODCallback.SOD_k_NO_CALLBACK),
        SODEntry(0x1004, 0x03, SODAttr([SODAttributes.RO], SODDataTypes.U32, None, None), 'SERR_aadwCommonEvtCtr[?INSTANCE?][SERR_k_SFS_FRM_ID]', None, SODCallback.SOD_k_NO_CALLBACK),
        SODEntry(0x1004, 0x04, SODAttr([SODAttributes.RO], SODDataTypes.U32, None, None), 'SERR_aadwCommonEvtCtr[?INSTANCE?][SERR_k_SFS_SADR_INV]', None, SODCallback.SOD_k_NO_CALLBACK),
        SODEntry(0x1004, 0x05, SODAttr([SODAttributes.RO], SODDataTypes.U32, None, None), 'SERR_aadwCommonEvtCtr[?INSTANCE?][SERR_k_SFS_SDN_INV]', None, SODCallback.SOD_k_NO_CALLBACK),
        SODEntry(0x1004, 0x06, SODAttr([SODAttributes.RO], SODDataTypes.U32, None, None), 'SERR_aadwCommonEvtCtr[?INSTANCE?][SERR_k_SFS_TADR_INV]', None, SODCallback.SOD_k_NO_CALLBACK),
        SODEntry(0x1004, 0x07, SODAttr([SODAttributes.RO], SODDataTypes.U32, None, None), 'SERR_aadwCommonEvtCtr[?INSTANCE?][SERR_k_SFS_CRC1]', None, SODCallback.SOD_k_NO_CALLBACK),
        SODEntry(0x1004, 0x08, SODAttr([SODAttributes.RO], SODDataTypes.U32, None, None), 'SERR_aadwCommonEvtCtr[?INSTANCE?][SERR_k_SFS_CRC2]', None, SODCallback.SOD_k_NO_CALLBACK),
        SODEntry(0x1004, 0x09, SODAttr([SODAttributes.RO], SODDataTypes.U32, None, None), 'SERR_aadwCommonEvtCtr[?INSTANCE?][SERR_k_SFS_DATA]', None, SODCallback.SOD_k_NO_CALLBACK),
        SODEntry(0x1004, 0x0A, SODAttr([SODAttributes.RO], SODDataTypes.U32, None, None), 'SERR_aadwCommonEvtCtr[?INSTANCE?][SERR_k_CYC_REJECT]', None, SODCallback.SOD_k_NO_CALLBACK),
        SODEntry(0x1004, 0x0B, SODAttr([SODAttributes.RO], SODDataTypes.U32, None, None), 'SERR_aadwCommonEvtCtr[?INSTANCE?][SERR_k_CYC_ERROR]', None, SODCallback.SOD_k_NO_CALLBACK),
        SODEntry(0x1004, 0x0C, SODAttr([SODAttributes.RO], SODDataTypes.U32, None, None), 'SERR_aadwCommonEvtCtr[?INSTANCE?][SERR_k_ACYC_REJECT]', None, SODCallback.SOD_k_NO_CALLBACK),
        SODEntry(0x1004, 0x0D, SODAttr([SODAttributes.RO], SODDataTypes.U32, None, None), 'SERR_aadwCommonEvtCtr[?INSTANCE?][SERR_k_ACYC_RETRY]', None, SODCallback.SOD_k_NO_CALLBACK),

        #Life Guarding
        SODEntryNumEntries(0x100C, 0x00, SODIndexComments.LifeGuarding),
        SODEntry(0x100C, 0x01, SODAttr([SODAttributes.RW, SODAttributes.CRC], SODDataTypes.U32, None, SODDefaultValue(value='10000000', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.lifeGuard.ulGuardTime', None, SODCallback.SOD_k_NO_CALLBACK),
        SODEntry(0x100C, 0x02, SODAttr([SODAttributes.RW, SODAttributes.CRC], SODDataTypes.U8, None, SODDefaultValue(value='2', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.lifeGuard.ucLifeTimeFactor', SODRange(None, None, 1, 255), SODCallback.SOD_k_NO_CALLBACK),

        #pre-operational signal
        SODEntry(0x100D, 0x00, SODAttr([SODAttributes.RW], SODDataTypes.U32, None, SODDefaultValue(value='100000', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.numRetriesRG.ulRefreshPreOp', None, SODCallback.SOD_k_NO_CALLBACK, SODIndexComments.NumberOfRetriesResetGuarding),

        #refresh interval reset guarding
        SODEntry(0x100E, 0x00, SODAttr([SODAttributes.RW], SODDataTypes.U8, None, SODDefaultValue(value='5', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.refreshIntRG.ucNoRetries', None, SODCallback.SOD_k_NO_CALLBACK, SODIndexComments.RefreshIntervalResetGuarding),

        #device vendor information
        SODEntryNumEntries(0x1018, 0x00, SODIndexComments.DeviceVendorInformation),
        SODEntry(0x1018, 0x01, SODAttr([SODAttributes.RO], SODDataTypes.U32, None, SODDefaultValue(value='CFG_SAPL_SN_VENDORID', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.devVendInfo.ulVendorId', None, SODCallback.SOD_k_NO_CALLBACK),
        SODEntry(0x1018, 0x02, SODAttr([SODAttributes.RO], SODDataTypes.U32, None, SODDefaultValue(value='CFG_SAPL_SN_PRODUCT_CODE', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.devVendInfo.ulProductCode', None, SODCallback.SOD_k_NO_CALLBACK),
        SODEntry(0x1018, 0x03, SODAttr([SODAttributes.RO], SODDataTypes.U32, None, SODDefaultValue(value='CFG_SAPL_SN_REVISION_NR', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.devVendInfo.ulRevisionNumber', None, SODCallback.SOD_k_NO_CALLBACK),
        SODEntry(0x1018, 0x04, SODAttr([SODAttributes.RO], SODDataTypes.U32, None, SODDefaultValue(value='CFG_SAPL_SN_SERIAL_NR', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.devVendInfo.ulSerialNumber', None, SODCallback.SOD_k_NO_CALLBACK),
        SODEntry(0x1018, 0x05, SODAttr([SODAttributes.RO], SODDataTypes.U32, None, SODDefaultValue(value='CFG_SAPL_SN_FW_CHKSUM', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.devVendInfo.ulFirmwareCrc', None, SODCallback.SOD_k_NO_CALLBACK),
        SODEntry(0x1018, 0x06, SODAttr([SODAttributes.RW], SODDataTypes.DOM, 'k_LEN_PARAM_CHKSUM_DOM', SODDefaultValue(datatype='tParamChksum', value='{ 0UL, {0UL}}', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), SOD_ActLenPtrData('s_?INSTANCE?_act_general.devVendInfo.aParamCrcs'), None, SODCallback.SOD_k_NO_CALLBACK),
        SODEntry(0x1018, 0x07, SODAttr([SODAttributes.RO], SODDataTypes.U32, None, SODDefaultValue(value='0x0', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.devVendInfo.aulParamTimestamp[0]', None, SODCallback.SOD_k_NO_CALLBACK),

        #unique device id
        SODEntry(0x1019, 0x00, SODAttr([SODAttributes.CONS], SODDataTypes.OCT, 'EPLS_k_UDID_LEN', SODDefaultValue(value='CFG_SAPL_SN_UDID', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), SOD_ActLenPtrData('s_?INSTANCE?_act_general.devVendInfo.aucUdid', sektor=SODDefines.SAFE_INIT_SEKTOR), None, SODCallback.SOD_k_NO_CALLBACK, SODIndexComments.UniqueDeviceId),

        #parameter download
        SODEntry(0x101A, 0x00, SODAttr([SODAttributes.WO, SODAttributes.AFT_WR], SODDataTypes.DOM, 'SAPL_k_MAX_PARAM_SET_LEN', None), SOD_ActLenPtrData(SODVariable(sektor=SODDefines.SAFE_NO_INIT_SEKTOR, process_later=True), '0U', sektor=SODDefines.SAFE_INIT_SEKTOR), None, SODCallback.SAPL_SOD_ParameterSet_CLBK, SODIndexComments.ParameterDownload),

        #common communication parameter
        SODEntryNumEntries(0x1200, 0x00, SODIndexComments.CommonCommunicationParameter),
        SODEntry(0x1200, 0x01, SODAttr([SODAttributes.RO], SODDataTypes.U16, None, SODDefaultValue(value='1', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.commonComParam.usSdn', SODRange(None, None, 0, 1023), SODCallback.SOD_k_NO_CALLBACK),
        SODEntry(0x1200, 0x02, SODAttr([SODAttributes.RO], SODDataTypes.U16, None, SODDefaultValue(value='0x01', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.commonComParam.usSadrOfScm', SODRange(None, None, 0, 1023), SODCallback.SOD_k_NO_CALLBACK),
        SODEntry(0x1200, 0x03, SODAttr([SODAttributes.RW, SODAttributes.CRC, SODAttributes.AFT_WR], SODDataTypes.I8, None, SODDefaultValue(value='2', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.commonComParam.cCtb', SODRange(None, None, 0, 3), SODCallback.SHNF_SOD_ConsTimeBase_CLBK),
        SODEntry(0x1200, 0x04, SODAttr([SODAttributes.RW], SODDataTypes.OCT, 'EPLS_k_UDID_LEN', SODDefaultValue(value='CFG_SAPL_SN_UDID', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), SOD_ActLenPtrData('s_?INSTANCE?_act_general.commonComParam.aucUdidScm[0]', sektor=SODDefines.SAFE_INIT_SEKTOR), None , SODCallback.SOD_k_NO_CALLBACK),

        #RxSPDO communication parameters
        SODEntryNumEntries(0x1400, 0x00, SODIndexComments.RxSpdoComParameter),
        SODEntry(0x1400, 0x01, SODAttr([SODAttributes.RW, SODAttributes.CRC], SODDataTypes.U16, None, SODDefaultValue(value='0x0000', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.aRxComParam[?SPDOPOS?].usSadr', SODRange(None, None, 0, 1023), None),
        SODEntry(0x1400, 0x02, SODAttr([SODAttributes.RW, SODAttributes.CRC], SODDataTypes.U32, None, SODDefaultValue(value='0x00000001UL', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.aRxComParam[?SPDOPOS?].ulSct', SODRange(None, None, 1, 65535), None),
        SODEntry(0x1400, 0x03, SODAttr([SODAttributes.RW, SODAttributes.CRC], SODDataTypes.U8, None, SODDefaultValue(value='0x01', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.aRxComParam[?SPDOPOS?].ucNoConsecTReq', SODRange(None, None, 1, 63), None),
        SODEntry(0x1400, 0x04, SODAttr([SODAttributes.RW, SODAttributes.CRC], SODDataTypes.U32, None, SODDefaultValue(value='0x00000000UL', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.aRxComParam[?SPDOPOS?].ulTimeDelayTReq', None, None),
        SODEntry(0x1400, 0x05, SODAttr([SODAttributes.RW, SODAttributes.CRC], SODDataTypes.U32, None, SODDefaultValue(value='0x00000001UL', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.aRxComParam[?SPDOPOS?].ulTimeDelaySync', None, None),
        SODEntry(0x1400, 0x06, SODAttr([SODAttributes.RW, SODAttributes.CRC], SODDataTypes.U16, None, SODDefaultValue(value='0x0001', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.aRxComParam[?SPDOPOS?].usMinTSyncPropDelay', SODRange(None, None, 1, 65535), None),
        SODEntry(0x1400, 0x07, SODAttr([SODAttributes.RW, SODAttributes.CRC], SODDataTypes.U16, None, SODDefaultValue(value='0x0001', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.aRxComParam[?SPDOPOS?].usMaxTSyncPropDelay', SODRange(None, None, 1, 65535), None),
        SODEntry(0x1400, 0x08, SODAttr([SODAttributes.RW, SODAttributes.CRC], SODDataTypes.U16, None, SODDefaultValue(value='0x0001', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.aRxComParam[?SPDOPOS?].usMinSpdoPropDelay', SODRange(None, None, 1, 65535), None),
        SODEntry(0x1400, 0x09, SODAttr([SODAttributes.RW, SODAttributes.CRC], SODDataTypes.U16, None, SODDefaultValue(value='0x0001', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.aRxComParam[?SPDOPOS?].usMaxSpdoPropDelay', SODRange(None, None, 1, 65535), None),
        SODEntry(0x1400, 0x0A, SODAttr([SODAttributes.RW, SODAttributes.CRC], SODDataTypes.U16, None, SODDefaultValue(value='0x0000', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.aRxComParam[?SPDOPOS?].usBestCaseTresDelay', SODRange(None, None, 0, 65535), None),
        SODEntry(0x1400, 0x0B, SODAttr([SODAttributes.RW, SODAttributes.CRC], SODDataTypes.U32, None, SODDefaultValue(value='0x00000001UL', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.aRxComParam[?SPDOPOS?].ulTReqCycle', None, None),
        SODEntry(0x1400, 0x0C, SODAttr([SODAttributes.RW, SODAttributes.CRC], SODDataTypes.U16, None, SODDefaultValue(value='0x01', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.aRxComParam[?SPDOPOS?].usTxSpdoNo', SODRange(None, None, 1, 1023), None),

        #RxSPDO mapping parameter, will be generated

        #TxSPDO communication parameters
        SODEntryNumEntries(0x1C00, 0x00, SODIndexComments.TxSpdoComParameter),
        SODEntry(0x1C00, 0x01, SODAttr([SODAttributes.RW, SODAttributes.CRC], SODDataTypes.U16, None, SODDefaultValue(value='0x0000', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.aTxComParam[?SPDOPOS?].usSadr', SODRange(None, None, 0, 1023), SODCallback.SOD_k_NO_CALLBACK),
        SODEntry(0x1C00, 0x02, SODAttr([SODAttributes.RW, SODAttributes.CRC], SODDataTypes.U16, None, SODDefaultValue(value='1', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.aTxComParam[?SPDOPOS?].usRefreshPrescale', SODRange(None, None, 1, 32767), SODCallback.SOD_k_NO_CALLBACK),
        SODEntry(0x1C00, 0x03, SODAttr([SODAttributes.RW, SODAttributes.CRC], SODDataTypes.U8, None, SODDefaultValue(value='0x00', sektor=SODDefines.SAFE_INIT_SEKTOR, process_later=True)), 's_?INSTANCE?_act_general.aTxComParam[?SPDOPOS?].ucNoTRes', None, SODCallback.SOD_k_NO_CALLBACK),

        #Vendor / module specific data
        SODEntry(0x2000, 0x00, SODAttr([SODAttributes.RW, SODAttributes.CRC], SODDataTypes.DOM, 'sizeof(tSettingsGroups)', None), SOD_ActLenPtrData('manParams', sektor=SODDefines.SAFE_INIT_SEKTOR), None, SODCallback.SOD_k_NO_CALLBACK, SODIndexComments.VendorModuleSpecificEntries),
        SODEntry(0x2001, 0x00, SODAttr([SODAttributes.RW, SODAttributes.CRC], SODDataTypes.DOM, 'sizeof(tUsedChannels)', None), SOD_ActLenPtrData('usedChannels.channel[0]', sektor=SODDefines.SAFE_INIT_SEKTOR), None, SODCallback.SOD_k_NO_CALLBACK),

        #User parameter, will ge benerated

        #Cyclic data, will be generated

        #TxSPDO mapping parameter, will be generated

        #end of SDO
        __sod_entry_end
        ])

    ##
    # \brief Returns the SOD end entry
    #
    # \return SODEntryEnd object of the default SOD
    @classmethod
    def get_end_entry(cls):
        return copy.deepcopy(cls.__sod_entry_end)

    ##
    # \brief Returns a list with SOD entries of the specified SODIndexRange
    # object
    #
    # \param sodindexrange an attribute of the class SODStandardObjects
    # \param add_index_offset if specified, an offset is added to the SOD indexes
    # before they are returned
    # \param append_endentry appends the SOD end entry to the returned list, if
    # True; no appendance otherwise
    # \return SODEntriesList
    @classmethod
    def get_default_sodentries_for_object(cls, sodindexrange,
                                          add_index_offset=0,
                                          append_endentry=False):

        return cls.get_default_sodentries_from_to(
                    sodindexrange.start, sodindexrange.end,
                    add_index_offset, append_endentry)

    ##
    # \brief Returns a list with SOD entries of the specified SODIndexRange
    # object
    #
    # \param startindex index from where SOD entries will be returned
    # \param endindex index (inclusive) to where SOD entries will be returned
    # \param add_index_offset if specified, an offset is added to the SOD indexes
    # before they are returned
    # \param append_endentry appends the SOD end entry to the returned list, if
    # True; no appendance otherwise
    # \return SODEntriesList
    @classmethod
    def get_default_sodentries_from_to(cls, startindex, endindex,
                                       add_index_offset=0,
                                       append_endentry=False):

        entries = cls.__sod_default_entries.get_entries_from_to(
                                        startindex, endindex, append_endentry)

        newlist = copy.deepcopy(entries)

        if add_index_offset > 0:
            for item in newlist:
                if not isinstance(item, SODEntryEnd) \
                and hasattr(item, 'objectindex'):
                    item.objectindex += add_index_offset

        return newlist

##
# \}
# \}
