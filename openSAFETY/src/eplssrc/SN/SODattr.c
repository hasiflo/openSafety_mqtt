/**
 * @addtogroup SOD
 * @{
 *
 * @file SODattr.c
 *
 * This file manages the address references to the SOD and the searching of the objects and attributes in the SOD.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 *
 * * <h2>History for SODattr.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>20.07.2010</td><td>Hans Pill</td><td>changes for LINT</td></tr>
 *     <tr><td>24.11.2010</td><td>Hans Pill</td><td>Review SL V20</td></tr>
 * </table>
 */

#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"

#include "SERRapi.h"
#include "SERR.h"

#include "SCFMapi.h"

#include "SODerr.h"
#include "SODapi.h"
#include "SODint.h"

#include "sacun.h"

/**
 * @var SAPL_ps_OD
 *  reference to the object dictionaries
 */
extern const SOD_t_OBJECT * const SAPL_ps_OD[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

/**
 * Object structure for the SODlock.c.
 */
typedef struct
{
    /** start address of the object dictionary */
    const SOD_t_OBJECT *ps_startOdAddr;
    /** end address of the object dictionary */
    const SOD_t_OBJECT *ps_endOdAddr;
    /** actual address in the object dictionary */
    const SOD_t_OBJECT *ps_actOdAddr;
    /** number of entries in the object  dictionary */
    UINT16 w_noOdEntries;
} t_SOD_ATTR_OBJ;

/**
 * @var as_Obj
 * Object structure for the SODlock.c.
 */
static t_SOD_ATTR_OBJ as_Obj[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

/***
 *    static function-prototypes
 ***/
static const SOD_t_OBJECT *SearchObject(const t_SOD_ATTR_OBJ *po_this,
                                        UINT16 w_idx, UINT8 b_subIdx,
                                        SOD_t_ABORT_CODES *pe_abortCode);

/**
* @brief This function initializes the addresses and number of entries.
*
* Start-, end- and actual address are set and number of SOD entries are calculated.
*
* @param        b_instNum         instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @return
* - TRUE            - success
* - FALSE           - failure
*/
BOOLEAN SOD_InitAddr(BYTE_B_INSTNUM)
{
    BOOLEAN o_ret = FALSE; /* return value */
    t_SOD_ATTR_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */
    const SOD_t_OBJECT *ps_tmpSodAddr; /* temporary pointer for the
     object dictionary checking */

    /* if the reference to the object dictionary is valid */
    if (SAPL_ps_OD[B_INSTNUMidx] != NULL )
    {
        /* start address of the object dictionary is initialized */
        po_this->ps_startOdAddr = SAPL_ps_OD[B_INSTNUMidx];

        /* actual address is initialized with start address of the SOD */
        po_this->ps_actOdAddr = po_this->ps_startOdAddr;

        po_this->w_noOdEntries = 0U;
        ps_tmpSodAddr = po_this->ps_startOdAddr;
        /* while the end of the SOD reached */
        while (ps_tmpSodAddr->w_index != SOD_k_END_OF_THE_OD )
        {
            /* increase the w_noEntriesInOD for calculating the length of the OD */
            po_this->w_noOdEntries++;
            /* step to the next object in the object dictionary */
            ps_tmpSodAddr++;
        }

        /* if the object dictionary is empty */
        if (po_this->w_noOdEntries == 0U)
        {
            SERR_SetError(B_INSTNUM_ SOD_k_ERR_OD_IS_EMPTY, (UINT32)B_INSTNUMidx);
        }
        /* else the object dictionary is not empty */
        else
        {
            /* end address of the object dictionary is initialized */
            po_this->ps_endOdAddr = po_this->ps_startOdAddr + po_this->w_noOdEntries;

            o_ret = TRUE;
        }
    }
    /* else the reference to the object dictionary is invalid */
    else
    {
        SERR_SetError(B_INSTNUM_ SOD_k_ERR_OD_REFERENCE, SERR_k_NO_ADD_INFO);
    }

        /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();

    return o_ret;
}

/**
 * @brief This function returns the attributes and the access handle of the via index and sub-index specified object.
 *
 * The handle and application object flag are necessary for all read/write accesses.
 *
 * @attention The returned data is READ-ONLY.
 *
 * @note This function is reentrant, therefore the Control Flow Monitoring is not active and SERR_SetError() is not called.
 * To make this function save, the return value ps_ret is initialized with NULL at the beginning of this function.
 * Additionally the error result is returned via ps_ret and ps_errRes.

 * @see          SOD_AttrGetNext()
 *
 * @param        b_instNum         instance number (checked), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_idx             index of object attributes to be accessed (checked), valid range: (UINT16)
 *
 * @param        b_subIdx          sub-index of object attributes to be accessed (checked), valid range: (UINT8)
 *
 * @retval          pdw_hdl           handle to the specified SOD entry (pointer checked), valid range : <> NULL
 *
 * @retval          po_appObj         application object flag
 * - TRUE  : application object
 * - FALSE : SOD object if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE) (pointer checked) if (SOD_cfg_APPLICATION_OBJ == EPLS_k_DISABLE) (not used)
 *
 * @retval          ps_errRes         error result, only relevant if the return value == NULL, (checked), valid range : <> NULL
 *
 * @return
 * - <> NULL         - pointer to the attributes of the SOD entry
 * - == NULL         - SOD entry not available or failure. For error  result see ps_errRes
 *
 */
const SOD_t_ATTR *SOD_AttrGet(BYTE_B_INSTNUM_ UINT16 w_idx, UINT8 b_subIdx,
		PTR_TYPE *pdw_hdl, BOOLEAN *po_appObj,
        SOD_t_ERROR_RESULT *ps_errRes)
{
    const SOD_t_ATTR *ps_ret = (SOD_t_ATTR *) NULL; /* return value */
    t_SOD_ATTR_OBJ *po_this; /* instance pointer */
    const SOD_t_OBJECT *ps_object; /* pointer to an object in the OD */
#if (SOD_cfg_APPLICATION_OBJ == EPLS_k_DISABLE)
    po_appObj = po_appObj; /* to avoid compiler warning */
#endif

    /* if the reference to the error result struture is NULL */
    if (ps_errRes == NULL )
    {
        /* return NULL, the error can not be signaled because the error result
         structure is NULL */
    }
#if (EPLS_cfg_MAX_INSTANCES > 1)
    /* else if b_instNum is wrong */
    else if(b_instNum >= (UINT8)EPLS_cfg_MAX_INSTANCES)
    {
        ps_errRes->w_errorCode = SOD_k_ERR_INST_NUM_1;
        ps_errRes->e_abortCode = SOD_ABT_GENERAL_ERROR;
    }
#endif
    /* else if invalid handle */
    else if (pdw_hdl == NULL )
    {
        ps_errRes->w_errorCode = SOD_k_ERR_OBJ_HDL_1;
        ps_errRes->e_abortCode = SOD_ABT_GENERAL_ERROR;
    }
#if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE)
    /* else if invalid handle */
    else if (po_appObj == NULL)
    {
        ps_errRes->w_errorCode = SOD_k_ERR_APP_OBJ_1;
        ps_errRes->e_abortCode = SOD_ABT_GENERAL_ERROR;
    }
#endif
    else /* else all parameter ok */
    {
        ps_errRes->w_errorCode = SOD_k_ERR_ATTR_GET;

        po_this = &as_Obj[B_INSTNUMidx];

        *pdw_hdl = (PTR_TYPE) (NULL );

        /* searching for the given object */
        ps_object = SearchObject(po_this, w_idx, b_subIdx,
                &ps_errRes->e_abortCode);

        /* if the specified object is available in the SOD object dictionary */
        if (ps_object != NULL )
        {
#if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE)
            /* signal to the SOD_Read/Write that the object is not application
             object */
            *po_appObj = FALSE;
#endif

            /* handle to specified SOD entry will be set */
            *pdw_hdl = (PTR_TYPE) (ps_object); /*lint !e923
             Note 923: cast from pointer to
             unsigned long [MISRA 2004 Rule 11.3] */

            /* return value will be set */
            ps_ret = &ps_object->s_attr;

            ps_errRes->w_errorCode = SOD_k_ERR_NO;
        }
        /* else the specified object is not available in the SOD object
         dictionary */
        else
        {
#if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE)
            /* if index does not exist */
            if (ps_errRes->e_abortCode == SOD_ABT_OBJ_DOES_NOT_EXIST)
            {

                ps_errRes->w_errorCode = SOD_k_ERR_SAPL_ATTR_GET;
                ps_errRes->e_abortCode = SOD_ABT_GENERAL_ERROR;

                /* signal to the SOD_Read/Write that the object is application
                 object */
                *po_appObj = TRUE;

                /* application callback function is called */
                ps_ret = SAPL_SOD_AttrGetClbk(B_INSTNUM_ w_idx, b_subIdx, pdw_hdl,
                        &ps_errRes->e_abortCode);

                /* if the access was OK */
                if (ps_ret != NULL)
                {
                    ps_errRes->w_errorCode = SOD_k_ERR_NO;
                    ps_errRes->e_abortCode = SOD_ABT_NO_ERROR;
                }
                /* no else : access was not OK, error happened */
            }
            /* no else sub-index does not exist */
#endif
        }
    }

    return ps_ret;
}

/**
 * @brief This function returns the attributes and the access handle of the via 'o_firstObj' specified SOD object.
 *
 * @attention The returned data is READ-ONLY. Application objects are not accessed.
 *
 * @note This function is NOT called by the openSAFETY stack and is NOT reentrant. The Control Flow Monitoring is not active
 * and SERR_SetError() is not called. To make this function save, the return value ps_ret is initialized with NULL at the
 * beginning of this function. Additionally the error result is returned via ps_ret and ps_errRes.

 * @see          SOD_AttrGet()
 *
 * @param        b_instNum          instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 * @param        o_firstObj
 * - TRUE : the return value points to the attribute of the first object.
 * - FALSE : the return value points to the attribute of the next object. (not checked, any value allowed) valid range: BOOLEAN
 * @retval          pdw_hdl            handle to the specified SOD entry (pointer checked) valid range: <> NULL
 * @retval          ps_errRes          error result, only relevant if the return value == NULL, (checked), valid range : <> NULL
 *
 * @return
 * - <> NULL          - pointer to the attributes of the SOD entry
 * - == NULL          - end of SOD reached ps_errRes =  SOD_k_ERR_NO, SOD_ABT_NO_ERROR or  failure. For error result see ps_errRes
 */
const SOD_t_ATTR *SOD_AttrGetNext(BYTE_B_INSTNUM_ BOOLEAN o_firstObj,
		PTR_TYPE *pdw_hdl,
                                  SOD_t_ERROR_RESULT *ps_errRes)
{
    const SOD_t_ATTR *ps_ret = (SOD_t_ATTR *) NULL; /* return value */
    t_SOD_ATTR_OBJ *po_this; /* instance pointer */

    /* if the reference to the error result struture is NULL */
    if (ps_errRes == NULL )
    {
        /* return NULL, the error can not be signaled because the error result
         struture is NULL */
    }
#if (EPLS_cfg_MAX_INSTANCES > 1)
    /* else if b_instNum is wrong */
    else if(b_instNum >= (UINT8)EPLS_cfg_MAX_INSTANCES)
    {
        ps_errRes->w_errorCode = SOD_k_ERR_INST_NUM_2;
        ps_errRes->e_abortCode = SOD_ABT_GENERAL_ERROR;
    }
#endif
    /* else if invalid handle */
    else if (pdw_hdl == NULL )
    {
        ps_errRes->w_errorCode = SOD_k_ERR_OBJ_HDL_2;
        ps_errRes->e_abortCode = SOD_ABT_GENERAL_ERROR;
    }
    else /* else all parameter ok */
    {
        ps_errRes->w_errorCode = SOD_k_ERR_NO;
        ps_errRes->e_abortCode = SOD_ABT_NO_ERROR;

        po_this = &as_Obj[B_INSTNUMidx];

        *pdw_hdl = (PTR_TYPE) (NULL );

        /* if first object */
        if (o_firstObj)
        {
            /* set the actual address to the start address */
            po_this->ps_actOdAddr = po_this->ps_startOdAddr;
            *pdw_hdl = (PTR_TYPE) (po_this->ps_actOdAddr); /*lint !e923
             Note 923: cast from pointer to
             unsigned long [MISRA 2004 Rule 11.3] */
            ps_ret = &po_this->ps_actOdAddr->s_attr;
        }
        /* else next object */
        else
        {
            /* set the actual address to the next object */
            po_this->ps_actOdAddr++;

            /* if the end of the SOD is reached */
            if (po_this->ps_actOdAddr == po_this->ps_endOdAddr)
            {
                /* the actual address points to the last object */
                po_this->ps_actOdAddr--;
            }
            /* else the end of the SOD is not reached */
            else
            {
                *pdw_hdl = (PTR_TYPE) (po_this->ps_actOdAddr); /*lint !e923
                 Note 923: cast from pointer to
                 unsigned long [MISRA 2004 Rule 11.3] */
                ps_ret = &po_this->ps_actOdAddr->s_attr;
            }
        }
    }

    return ps_ret;
}

/**
 * @brief This function checks the handle of the SOD entry.
 *
 * @param        b_instNum            instance number (not checked, checked in SOD_ActualLenSet() or SOD_Write() or SOD_Read() or SOD_ActualLenGet()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        dw_hdl              object handle (checked)
 *                                    valid range: UINT32
 * @retval       ps_errRes
 * - == NULL error is generate in the function
 * - <> NULL error result, only relevant if the return value == FALSE, (pointer not checked, any value allowed), valid range : <> or == NULL
 *
 * @return
 * - TRUE               - SOD entry handle is valid
 * - FALSE              - SOD entry handle is not valid, see ps_errRes
 */
BOOLEAN SOD_HdlValid(BYTE_B_INSTNUM_ PTR_TYPE dw_hdl,
        SOD_t_ERROR_RESULT *ps_errRes)
        {
    BOOLEAN o_ret = FALSE; /* return value */
    t_SOD_ATTR_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */
    /* temporary variable to increase the readability of the code */
    PTR_TYPE dw_tmpStartAddr = (PTR_TYPE) (po_this->ps_startOdAddr); /*lint !e923
     Note 923: cast from pointer to unsigned long
     [MISRA 2004 Rule 11.3] */
    /* temporary variable to increase the readability of the code */
    PTR_TYPE dw_tmpEndAddr = (PTR_TYPE) (po_this->ps_endOdAddr); /*lint !e923
     Note 923: cast from pointer to unsigned long
     [MISRA 2004 Rule 11.3] */


    /* if the handle of the SOD entry is valid and the dw_hdl address points to
     an object exactly */
    if ((dw_tmpStartAddr <= dw_hdl) && (dw_tmpEndAddr > dw_hdl) &&
            (((dw_hdl - dw_tmpStartAddr) % sizeof(SOD_t_OBJECT)) == 0U))
    {
        o_ret = TRUE;
    }
    /* else the handle of the SOD entry is invalid */
    else
    {
        /* if error has to be generate by this function */
        if (ps_errRes == NULL )
        {
            SERR_SetError(B_INSTNUM_ SOD_k_ERR_OBJ_HDL_INVALID,
            (UINT32)SOD_ABT_LOCAL_CONTROL);
        }
        else /* error is not genereted by this function */
        {
            ps_errRes->w_errorCode = SOD_k_ERR_OBJ_HDL_INVALID;
            ps_errRes->e_abortCode = SOD_ABT_LOCAL_CONTROL;
        }
    }

    return o_ret;
}

/**
* @brief This function gets a pointer to the first SOD object.
*
* @param      b_instNum      instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @return     Pointer to the first SOD object.
*/
const SOD_t_OBJECT *SOD_GetSodStartAddr(BYTE_B_INSTNUM)
{
    t_SOD_ATTR_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */

    /* call the Control Flow Monitoring */SCFM_TACK_PATH();

    return po_this->ps_startOdAddr;
}


/**
 * @brief This function searches a via index/sub-index specified object in the SOD and returns a pointer to the object.
 *
 * @param        po_this              pointer to the module instance(not checked)valid range: <> NULL
 *
 * @param        w_idx                index of object attributes to be read(not checked, any value allowed) valid range: (UINT16)
 *
 * @param        b_subIdx             sub-index of object attributes to be read(not checked, any value allowed) valid range: (UINT8)
 *
 * @retval          pe_abortCode         pointer to the abort code(pointer not checked, only called with reference to enum in SOD_AttrGet()) valid range: <> NULL
 *
 * @return
 * - <> NULL            - pointer to the SOD entry
 * - == NULL            - SOD entry not available, see pe_abortCode
 */
static const SOD_t_OBJECT *SearchObject(const t_SOD_ATTR_OBJ *po_this,
                                        UINT16 w_idx, UINT8 b_subIdx,
                                        SOD_t_ABORT_CODES *pe_abortCode)
{
    const SOD_t_OBJECT *ps_ret = (SOD_t_OBJECT *) NULL; /* return value */
    const SOD_t_OBJECT *ps_tmpEntry = po_this->ps_startOdAddr; /* temporary
     pointer to an object entry */
    BOOLEAN o_idxFound = FALSE; /* boolean for signaling whether the index was
     found or sub-index */
    BOOLEAN o_subIdxFound = FALSE; /* boolean for signaling whether the sub-index
     was found or sub-index */

    /*          Object Dictionary
     =================== w_low (low limit of the search range)
     |          |
     |          |
     |          +-- w_halfOfNoElem
     |          |   (half of number of elements in the search
     w_noElem ----------+          |    range)
     (number of elements  |
     in the search range)|
     |
     |
     =================== high limit of the search range
     */
    UINT16 w_low = 0U; /* low limit of the search range */
    UINT16 w_halfOfNoElem; /* half of number of elements in the search range */
    UINT16 w_noElem = po_this->w_noOdEntries; /* number of elements in the search
     range */
    UINT16 w_cmpIdx; /* index to be compared */
    UINT8 b_cmpSubIdx; /* sub-index to be compared */

    *pe_abortCode = SOD_ABT_NO_ERROR;

    /* Algorithm for the binary search:
     while the search range is not empty and
     the index or the sub-index were not found */
    while ((w_noElem > 0U) && ((!o_idxFound) || (!o_subIdxFound)))
    {
        w_halfOfNoElem = (UINT16) (w_noElem >> 1);
        w_cmpIdx = ps_tmpEntry[w_halfOfNoElem + w_low].w_index;

        /* if index was found */
        if (w_cmpIdx == w_idx)
        {
            /* index is found */
            o_idxFound = TRUE;

            b_cmpSubIdx = ps_tmpEntry[w_halfOfNoElem + w_low].b_subIndex;
            /* if sub-index was found */
            if (b_cmpSubIdx == b_subIdx)
            {
                o_subIdxFound = TRUE;
                /* object has been found */
                ps_ret = &ps_tmpEntry[w_halfOfNoElem + w_low];
            }
            /* else if searched sub-index is smaller */
            else if (b_cmpSubIdx > b_subIdx)
            {
                w_noElem = w_halfOfNoElem;
            }
            /* else searched sub-index is larger */
            else
            {
                /* SOD object sub-index is larger than searched sub-index and this SOD
                 sub-index is ignored in the following search range. */
                w_halfOfNoElem++;
                w_low = (UINT16) (w_halfOfNoElem + w_low);
                w_noElem = (UINT16) (w_noElem - w_halfOfNoElem);
            }
        }
        /* else if searched index is smaller */
        else if (w_cmpIdx > w_idx)
        {
            w_noElem = w_halfOfNoElem;
        }
        /* else searched index is larger */
        else
        {
            /* SOD object index is larger than searched sub-index and this SOD index
             is ignored in the following search range. */
            w_halfOfNoElem++;
            w_low = (UINT16) (w_halfOfNoElem + w_low);
            w_noElem = (UINT16) (w_noElem - w_halfOfNoElem);
        }
    }

    /* if the object was not found */
    if (ps_ret == NULL )
    {
        /* if the given sub-index was not found */
        if (o_idxFound)
        {
            *pe_abortCode = SOD_ABT_SUB_IDX_DOES_NOT_EXIST;
        }
        /* else the given index was not found */
        else
        {
            *pe_abortCode = SOD_ABT_OBJ_DOES_NOT_EXIST;
        }
    }
    /* no else : object was found */

    return ps_ret;
}

/** @} */
