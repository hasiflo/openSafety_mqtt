#ifndef SHNFMSG_H
#define SHNFMSG_H

#include "EPLStarget.h"

#include "EPLScfg.h"
#include "EPLStypes.h"

#include "SHNF.h"
#include "SHNF_Definitions.h"

#include "MQTT.h"

void InsertIntoRXBuffer ( const UINT8 * c_receivingBuffer, UINT16 ui_numberOfBytes );

UINT8 * SHNF_GetTxMemBlock(BYTE_B_INSTNUM_ UINT16 w_blockSize, SHNF_t_TEL_TYPE e_telType, UINT16 w_txSpdoNum);

/**
 * \brief This function initializes the SHNF interface.
 *
 * \param w_loopBackSadr - loop back source address
 */
void SHNF_Init(BYTE_B_INSTNUM_ UINT16 w_loopBackSadr);

/**
 * This function returns a reference to a received openSAFETY frame.
 *
 * The reserved space for each received frame can be freed by calling the
 * \see SHNF_ReleaseEplsFrame function.
 *
 * \retval pw_frameLength the number of bytes for the received openSAFETY frame
 * \retval e_telType the @see SHNF_t_TEL_TYPE type of the received openSAFETY frame
 *
 * \return - == NULL - no openSAFETY frame available
 *         - != NULL - pointer to the received openSAFETY frame
 */
UINT8 * SHNF_GetEplsFrame(UINT16 *pw_frameLength, SHNF_t_TEL_TYPE *e_telType);

/**
 * \brief This function releases an EPLS frame which is no longer needed.
 * \see SHNF_GetEplsFrame
 */
void SHNF_ReleaseEplsFrame(void);

/**
 * \brief This function provides cleanup functionality for the network interface.
 */
void SHNF_CleanUp();


#endif
