/**
 * @addtogroup SOD
 * @{
 *
 * @file SODlock.c
 *
 * This file manages the locking and the locking of the SOD.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 */

#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"

#include "SERRapi.h"
#include "SERR.h"

#include "SCFMapi.h"

#include "SODerr.h"
#include "SODapi.h"
#include "SOD.h"
#include "SODint.h"

/**
 * Object structure for the SODlock.c.
 */
typedef struct
{
    /** flag for locking all segmented objects */
    BOOLEAN o_locked;
    /** flag to disable the SOD write access */
    BOOLEAN o_sodWrEnabled;
} t_SOD_LOCK_OBJ;

static t_SOD_LOCK_OBJ as_Obj[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

/**
* @brief This function initializes the flags used to lock and disable the SOD.
*
* @param        b_instNum         instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*/
void SOD_InitFlags(BYTE_B_INSTNUM)
{
    t_SOD_LOCK_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */

    /* lock flag is set to FALSE */
    po_this->o_locked = FALSE;
    /* SOD write access is enabled */
    po_this->o_sodWrEnabled = TRUE;

    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();
}

/**
 *
 */
BOOLEAN SOD_Lock(BYTE_B_INSTNUM_ PTR_TYPE dw_hdl, BOOLEAN o_appObj)
{
    BOOLEAN o_ret = FALSE; /* return value */
    t_SOD_LOCK_OBJ *po_this; /* instance pointer */
#if (SOD_cfg_APPLICATION_OBJ == EPLS_k_DISABLE)
    o_appObj = o_appObj; /* to avoid compiler warning */
    dw_hdl = dw_hdl; /* to avoid compiler warning */
#endif

#if (EPLS_cfg_MAX_INSTANCES > 1)
    /* if b_instNum is wrong */
    if(b_instNum >= (UINT8)EPLS_cfg_MAX_INSTANCES)
    {
        SERR_SetError(EPLS_k_NO_INSTANCE, SOD_k_ERR_INST_NUM_5,
                (UINT32)SOD_ABT_GENERAL_ERROR);
    }
    else /* else all parameter ok */
#endif
    {
        po_this = &as_Obj[B_INSTNUMidx];

#if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE)
        /* if application object */
        if (o_appObj)
        {
            o_ret = SAPL_SOD_LockClbk(B_INSTNUM_ dw_hdl);

            /* if the locking failed */
            if (!o_ret)
            {
                SERR_SetError(B_INSTNUM_ SOD_k_ERR_SAPL_LOCKING_FAILED,
                        (UINT32)SOD_ABT_PRESENT_DEVICE_STATE);
            }
            /* no else : locking was successful, no error happened */
        }
        /* else not application object */
        else
#endif
        {
            /* if access to the segmented objects is locked */
            if (po_this->o_locked)
            {
                SERR_SetError(B_INSTNUM_ SOD_k_ERR_LOCKING_FAILED,
                (UINT32)SOD_ABT_PRESENT_DEVICE_STATE);
            }
            /* else access to the segmented objects is not locked */
            else
            {
                /* lock flag is set to lock the access to the segmented objects */
                po_this->o_locked = TRUE;

                o_ret = TRUE;
            }
        }
    }

                /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();

    return o_ret;
}

/**
 *
 */
BOOLEAN SOD_Unlock(BYTE_B_INSTNUM_ PTR_TYPE dw_hdl, BOOLEAN o_appObj)
{
    BOOLEAN o_ret = FALSE; /* return value */
    t_SOD_LOCK_OBJ *po_this; /* instance pointer */
#if (SOD_cfg_APPLICATION_OBJ == EPLS_k_DISABLE)
    o_appObj = o_appObj; /* to avoid compiler warning */
    dw_hdl = dw_hdl;
#endif

#if (EPLS_cfg_MAX_INSTANCES > 1)
    /* if b_instNum is wrong */
    if(b_instNum >= (UINT8)EPLS_cfg_MAX_INSTANCES)
    {
        SERR_SetError(EPLS_k_NO_INSTANCE, SOD_k_ERR_INST_NUM_6,
                (UINT32)SOD_ABT_GENERAL_ERROR);
    }
    else /* else all parameter ok */
#endif
    {
        po_this = &as_Obj[B_INSTNUMidx];

#if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE)
        /* if application object */
        if (o_appObj)
        {
            o_ret = SAPL_SOD_UnlockClbk(B_INSTNUM_ dw_hdl);

            /* if the unlocking failed */
            if (!o_ret)
            {
                SERR_SetError(B_INSTNUM_ SOD_k_ERR_SAPL_UNLOCK_FAILED,
                        (UINT32)SOD_ABT_PRESENT_DEVICE_STATE);
            }
        }
        /* else not application object */
        else
#endif
        {
            /* if calling SOD_Lock was successful */
            if (po_this->o_locked)
            {
                /* lock flag is cleared to unlock the access to the segmented
                 objects */
                po_this->o_locked = FALSE;
                o_ret = TRUE;
            }
            /* else calling SOD_Lock failed */
            else
            {
                SERR_SetError(B_INSTNUM_ SOD_k_ERR_UNLOCK_WITHOUT_LOCK,
                (UINT32)SOD_ABT_PRESENT_DEVICE_STATE);
            }
        }
    }

                /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();

    return o_ret;
}

/**
 * @brief This function gets whether the SOD is locked or not.
 *
 * @param        b_instNum         instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame())
 *      valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @return
 * - TRUE            - SOD is locked
 * - FALSE           - SOD is not locked
 */
BOOLEAN SOD_IsLocked(BYTE_B_INSTNUM)
{
    t_SOD_LOCK_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */

    /* call the Control Flow Monitoring */SCFM_TACK_PATH();

    return po_this->o_locked;
}

/**
 * @brief This function enables the SOD write access.
 *
 * @note e.g.: After the generation of the CRC or the parameter storage.
 *
 * @see          SOD_DisableSodWrite()
 *
 * @param        b_instNum         instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @return
 * - TRUE            - success
 * - FALSE           - failure
 */
BOOLEAN SOD_EnableSodWrite(BYTE_B_INSTNUM)
{
    BOOLEAN o_ret = FALSE; /* return value */
    t_SOD_LOCK_OBJ *po_this; /* instance pointer */

#if (EPLS_cfg_MAX_INSTANCES > 1)
    /* if b_instNum is wrong */
    if(b_instNum >= (UINT8)EPLS_cfg_MAX_INSTANCES)
    {
        SERR_SetError(EPLS_k_NO_INSTANCE, SOD_k_ERR_INST_NUM_7,
                (UINT32)SOD_ABT_GENERAL_ERROR);
    }
    else /* else all parameter ok */
#endif
    {
        po_this = &as_Obj[B_INSTNUMidx];

        /* if the SOD write access is disabled */
        if (!po_this->o_sodWrEnabled)
        {
            /* The SOD write access is enabled */
            po_this->o_sodWrEnabled = TRUE;

            o_ret = TRUE;
        }
        /* else the SOD write access was not disabled */
        else
        {
            SERR_SetError(B_INSTNUM_ SOD_k_ERR_ENABLE_WR_FAILED,
            (UINT32)SOD_ABT_GENERAL_ERROR);
        }
    }

            /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();

    return o_ret;
}

/**
 * @brief This function disables the SOD write access.
 *
 * @note e.g. Before the generation of the CRC or the parameter storage.
 *
 * @see SOD_EnableSodWrite()
 *
 * @param        b_instNum         instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @return
 * - TRUE            - success
 * - FALSE           - failure (in case of SOD_k_ERR_ENABLE_WR_FAILED the SOD write is already disabled by another process)
 */
BOOLEAN SOD_DisableSodWrite(BYTE_B_INSTNUM)
{
    BOOLEAN o_ret = FALSE; /* return value */
    t_SOD_LOCK_OBJ *po_this; /* instance pointer */

#if (EPLS_cfg_MAX_INSTANCES > 1)
    /* if b_instNum is wrong */
    if(b_instNum >= (UINT8)EPLS_cfg_MAX_INSTANCES)
    {
        SERR_SetError(EPLS_k_NO_INSTANCE, SOD_k_ERR_INST_NUM_8,
                (UINT32)SOD_ABT_GENERAL_ERROR);
    }
    else /* else all parameter ok */
#endif
    {
        po_this = &as_Obj[B_INSTNUMidx];

        /* if the SOD write access is enabled and SOD is not locked */
        if ((po_this->o_sodWrEnabled) && (!po_this->o_locked))
        {
            /* The SOD write access is disabled */
            po_this->o_sodWrEnabled = FALSE;

            o_ret = TRUE;
        }
        /* else the SOD write access is already disabled */
        else
        {
            SERR_SetError(B_INSTNUM_ SOD_k_ERR_DISABLE_WR_FAILED,
            (UINT32)SOD_ABT_GENERAL_ERROR);
        }
    }

            /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();

    return o_ret;
}

/**
 * @brief This function gets whether the write access to the SOD is enabled or disabled.
 *
 * @param b_instNum instance number (not checked, checked in SOD_Write() or SSC_ProcessSNMTSSDOFrame())
 *      valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @return
 * - TRUE            - SOD write access is enabled
 * - FALSE           - SOD write access is disabled
 */
BOOLEAN SOD_WriteAccess(BYTE_B_INSTNUM)
{
    t_SOD_LOCK_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */

    /* call the Control Flow Monitoring */SCFM_TACK_PATH();

    return po_this->o_sodWrEnabled;
}

/** @} */
