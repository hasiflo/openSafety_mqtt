/**
 * \file crc_protocol.c
 * \brief Reference implementation for openSAFETY CRC-Calculations
 *
 * \details This programm should be a reference implementation for the two openSAFETY CRC-Calculations
 * It is possible to compare the implemented CRC�s with the mentioned reference values from this
 * programm. This programm is not optimized for performance.
 * The programm depends on an 8 Bit/Byte machine. unsigned int has to have 16 bits or more.
 *
 * \details Summary:
 * 1. Append zero bits to the data stream
 * 2. Devide the complete data stream by using CRC arithmetic
 * 3. The reminder is the checksum
 *
 * \version openSAFETY V1.0
 * \author M. Kieviet innotec GmbH
 * Review: Anton Graf, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H.
 */

/*
 * \file crc.c
 * \version 1.0
 */

#include <stdio.h>
#include <oschecksum/crc.h>

/* prototype of left shifting by one bit */
static unsigned char uc_array_shift_left(unsigned char *, unsigned int);

/**
 * \fn uc_openSAFETY_CRC8()
 * \version 1.0
 * \date 2005-04-22
 * \author m.kieviet
 *
 * \brief calculate the CRC8 checksum with example from
 * the openSAFETY specification. Maximum datalength of 14 byte
 *
 * \param ui_sub_length is an integer frame length
 * \param ui_poly the integer polynomial which will be in use
 * \param uc_subframe pointer to start address of frame array
 *
 * \return unsinged char checksum
 */
uint8_t uc_openSAFETY_CRC8(uint8_t uc_sub_length, uint8_t uc_poly, uint8_t * uc_subframe)
{
    unsigned char uc_checksum; /* reminder as crc checksum     */
    unsigned char uc_i; /* counter variable             */
    unsigned int ui_stream_le; /* stream length                */
    unsigned char uc_data[15]; /* include the complete stream
     with appended zeros         */
    /*array is not optimized!       */

    for (uc_i = 0; uc_i < uc_sub_length; uc_i++)
    {
        uc_data[uc_i] = uc_subframe[uc_i]; /*copy the frame in stream and */
    }
    uc_data[uc_sub_length] = 0; /*append the zeros*/
    ui_stream_le = uc_sub_length + 1;
    /* CRC calculation by shifting and xor */
    for (uc_i = 0; uc_i < uc_sub_length * 8; uc_i++)
    {
        if (uc_array_shift_left(&uc_data[0], ui_stream_le))
        {
            /* XOR when bit falling out is not zero */
            uc_data[0] = uc_data[0] ^ uc_poly;
        }
    }

    /* result*/
    uc_checksum = uc_data[0];
    return (uc_checksum);
}

/**
 * \fn uc_openSAFETY_CRC16()
 * \version 1.0
 * \date 2005-04-22
 * \author m.kieviet
 *
 * \brief calculate the CRC8 checksum with example from
 * the openSAFETY specification. Maximum datalength of 14 byte
 *
 * \param ui_sub_length is an integer frame length
 * \param ui_poly the integer polynomial which will be in use
 * \param uc_subframe pointer to start address of frame array
 *
 * \return unsigned integer checksum
 */
uint16_t ui_openSAFETY_CRC16(uint16_t ui_sub_length, uint16_t ui_poly, uint8_t *uc_subframe)
{
    uint16_t ui_crc_sum = 0;
    uint16_t ui_i;
    uint16_t ui_stream_le; /* stream length */
    uint8_t uc_data[263];
    uint8_t uc_poly[2];

    uc_poly[1] = (uint8_t) ui_poly; /*LSB*/
    uc_poly[0] = (uint8_t) ((uint16_t)ui_poly >> 8);  /*MSB*/

    for (ui_i = 0; ui_i < ui_sub_length; ui_i++)
    {
        uc_data[ui_i] = uc_subframe[ui_i]; /* copy the frame in stream and */
    }
    uc_data[ui_sub_length] = 0; /* append the zeros */
    uc_data[ui_sub_length + 1] = 0; /* append the zeros */
    ui_stream_le = ui_sub_length + 2;
    /* CRC calculation by shifting and xor */
    for (ui_i = 0; ui_i < ( (uint16_t)ui_sub_length  * 8 );  ui_i++)
    {
        if (uc_array_shift_left(&uc_data[0], ui_stream_le))
        {
            /* XOR when bit falling out is not zero */
            uc_data[0] = uc_data[0] ^ uc_poly[0];
            uc_data[1] = uc_data[1] ^ uc_poly[1];
        }
    }

    /* result */
    ui_crc_sum = uc_data[0];
    ui_crc_sum <<= 8;
    ui_crc_sum = ui_crc_sum | uc_data[1];

    return (ui_crc_sum);
}

/**
 * \fn uc_array_shift_left()
 * \version 1.0
 * \date 2005-04-22
 * \author a.graf
 *
 * \brief Should shift bit by bit to the left. maximum datalength of UINT_MAX
 *
 * \param pointer start address of stream array
 * \param integer field length
 *
 * \return unsigned char value of bit shifted out at left
 */
unsigned char uc_array_shift_left(unsigned char *uc_info,
                unsigned int us_fields)
{
    signed int si_i = 0;        /* count variable */
    unsigned char msb = 0;      /* buffer */
    unsigned char carry = 0;    /* carry  */

    for (si_i = us_fields - 1; si_i >= 0; si_i--) /* shift entire array from right to left */
    {
        msb = uc_info[si_i] & 0x80;
        msb >>= 7;

        uc_info[si_i] <<= 1;    /* shift left   */
        uc_info[si_i] |= carry; /* add carry    */
        carry = msb;            /* msb to next carry*/
    }

    return (carry);
}

