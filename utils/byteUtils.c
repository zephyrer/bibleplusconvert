//Generate by python.
/**
 * @file
 * @brief Bytes encode/decode.
 *
 * @details 
 *      This module encode and decode bytes.
 *
 * @author  buaa.byl@gmail.com
 * @date    2011.01.10
 * @version 0.0.1.alpha
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "byteUtils.h"

#ifdef _MSC_VER
#define __func__ __FUNCTION__
#endif

///////////////////////////////////////////////////////////////////////////////
//Local Macro define
#define BYTE_BE2LE_U16(u16) \
    ((((u16) << 8) & 0xFF00u) | (((u16) >> 8) & 0x00FFu))

#define BYTE_LE2BE_U16(u16) BYTE_BE2LE_U16(u16)

#define BIT_GET_BIT(u, i) \
    (((u) >> i) & 0x1u)

#define BIT_SET_BIT(u, i) \
    ((u) |= (0x1u << (i)))

#define BIT_CLR_BIT(u, i) \
    ((u) &= (~(0x1u << (i))))

#define BIT_REVERSE_U16(u16) \
    ((BIT_GET_BIT(u16, 0) << 15) |\
     (BIT_GET_BIT(u16, 1) << 14) |\
     (BIT_GET_BIT(u16, 2) << 13) |\
     (BIT_GET_BIT(u16, 3) << 12) |\
     (BIT_GET_BIT(u16, 4) << 11) |\
     (BIT_GET_BIT(u16, 5) << 10) |\
     (BIT_GET_BIT(u16, 6) <<  9) |\
     (BIT_GET_BIT(u16, 7) <<  8) |\
     (BIT_GET_BIT(u16, 8) <<  7) |\
     (BIT_GET_BIT(u16, 9) <<  6) |\
     (BIT_GET_BIT(u16, 10) << 5) |\
     (BIT_GET_BIT(u16, 11) << 4) |\
     (BIT_GET_BIT(u16, 12) << 3) |\
     (BIT_GET_BIT(u16, 13) << 2) |\
     (BIT_GET_BIT(u16, 14) << 1) |\
     (BIT_GET_BIT(u16, 15) << 0) \
     )

#define BIT_REVERSE_U14(u14) \
    (BIT_REVERSE_U16(u14) >> 2)

#define BIT_REVERSE_U8(u8) \
    (BIT_REVERSE_U16(u8) >> 8)

#if (HOSTPC_ENDIAN != TARGET_ENDIAN)
#define ENDIAN_CONVERT_U14(u14) BIT_REVERSE_U14(u14);
#define ENDIAN_CONVERT_U16(u16) BIT_REVERSE_U16(u16);
#else
#define ENDIAN_CONVERT_U14(u14) (u14)
#define ENDIAN_CONVERT_U16(u16) (u16)
#endif


#define ARGUMENT_CHECK_NOT_NULL(p) \
    if ((p) == NULL)\
    {\
        printf("Error : pointer in %s is NULL\n", __func__);\
        return BSB_FALSE;\
    }

///////////////////////////////////////////////////////////////////////////////
//Local const variable and struct define
struct ByteStreamBit
{
    unsigned short  bitsArray[BYTESTREAM16BIT_MAX];
    int             nrOfBits;
};


///////////////////////////////////////////////////////////////////////////////
//Global interface define
struct ByteStreamBit* ByteStreamBit_Init(void)
{
    struct ByteStreamBit* pByteStream = NULL;
    int i = 0;

    pByteStream = (struct ByteStreamBit*)malloc(sizeof(struct ByteStreamBit));
    if (pByteStream != NULL)
    {
        pByteStream->nrOfBits = 0;
        for (i = 0;i < BYTESTREAM16BIT_MAX;i++)
        {
            pByteStream->bitsArray[i] = 0u;
        }
    }
    else
    {
        printf("Error : Can't malloc new item!\n");
    }

    return pByteStream;
}

BSB_RESULT ByteStreamBit_Copy(
        const struct ByteStreamBit* pSrc,
        struct ByteStreamBit* pDst
)
{
    int i = 0;

    if ((pSrc != NULL) && (pDst != NULL))
    {
        pDst->nrOfBits = pSrc->nrOfBits;
        for (i = 0;i < BYTESTREAM16BIT_MAX;i++)
        {
            pDst->bitsArray[i] = pSrc->bitsArray[i];
        }

        return BSB_TRUE;
    }
    else
    {
        return BSB_FALSE;
    }
}

BSB_RESULT ByteStreamBit_Zero(struct ByteStreamBit* pByteStream)
{
    int i = 0;
    if (pByteStream != NULL)
    {
        pByteStream->nrOfBits = 0;
        for (i = 0;i < BYTESTREAM16BIT_MAX;i++)
        {
            pByteStream->bitsArray[i] = 0u;
        }
        return BSB_TRUE;
    }
    else
    {
        return BSB_FALSE;
    }
}

BSB_RESULT ByteStreamBit_Fini(struct ByteStreamBit* pByteStream)
{
    if (pByteStream != NULL)
    {
        free(pByteStream);
        pByteStream = NULL;
        return BSB_TRUE;
    }
    else
    {
        return BSB_FALSE;
    }
}

BSB_RESULT ByteStreamBit_AppendU14(
        struct ByteStreamBit* pByteStream, 
        unsigned short u14)
{
    int indexOfUseBytes = 0;
    int nrOfUseBits = 0;
    int indexOfNewBytes = 0;
    int nrOfNewBits = 0;
    int nrOfLeftBits = 0;

    ARGUMENT_CHECK_NOT_NULL(pByteStream)

    u14 = BIT_REVERSE_U14(u14);

    if (pByteStream->nrOfBits < (BYTESTREAM14BIT_MAX * 14))
    {
        u14 &= BYTESTREAM14BIT_MASK;
        indexOfUseBytes = pByteStream->nrOfBits / 16;
        nrOfUseBits     = pByteStream->nrOfBits % 16;

        indexOfNewBytes = indexOfUseBytes;
        if (nrOfUseBits == 0)
        {
            //All bits of last byte used.
            nrOfNewBits     = 14;
        }
        else
        {
            //There are (16 - nrOfUseBits) left.
            nrOfNewBits     = 16 - nrOfUseBits;
        }

        nrOfLeftBits = 14 - nrOfNewBits;

        //Clear none used bits
        pByteStream->bitsArray[indexOfNewBytes] &= 
            ~(0xFFFFu << nrOfUseBits);

        //Copy bits
        pByteStream->bitsArray[indexOfNewBytes] |= 
            (u14 & (~((0xFFFFu << nrOfNewBits)))) << (nrOfUseBits);

        //Copy remaind bits
        if (nrOfLeftBits > 0)
        {
            indexOfNewBytes++;
            //Clear new bits
            pByteStream->bitsArray[indexOfNewBytes] &= 
                (0xFFFFu << nrOfLeftBits);

            //Copy left bits
            pByteStream->bitsArray[indexOfNewBytes] |= 
                (u14 >> nrOfNewBits) & (~((0xFFFFu << nrOfLeftBits)));
        }

        pByteStream->nrOfBits += 14;

        return BSB_TRUE;
    }
    else
    {
        printf("Error : pByteStream full!\n");
        return BSB_FALSE;
    }
}

BSB_RESULT ByteStreamBit_AppendU16(
        struct ByteStreamBit* pByteStream, 
        unsigned short u16)
{
    int index = 0;

    ARGUMENT_CHECK_NOT_NULL(pByteStream)

    u16 = BIT_REVERSE_U16(u16);

    if (pByteStream->nrOfBits < (BYTESTREAM16BIT_MAX * 16))
    {
        u16 &= BYTESTREAM16BIT_MASK;
        index = pByteStream->nrOfBits / 16;
        pByteStream->bitsArray[index] = u16;
        pByteStream->nrOfBits += 16;

        return BSB_TRUE;
    }
    else
    {
        printf("Error : pByteStream full!\n");
        return BSB_FALSE;
    }
}

unsigned short ByteStreamBit_GetU14(
        const struct ByteStreamBit* pByteStream,
        unsigned short index)
{
    unsigned short u14 = 0u;
    unsigned short tmp = 0u;
    int indexOfUseBytes = 0;
    int offsetOfStartBit = 0;
    int offsetOfBits = 0;
    int nrOfLeftBits = 0;
    int nrOfBitsInFistByte = 0;
    
    ARGUMENT_CHECK_NOT_NULL(pByteStream)

    if (index < BYTESTREAM14BIT_MAX)
    {
        offsetOfBits        = index * 14;
        indexOfUseBytes     = offsetOfBits / 16;
        offsetOfStartBit    = offsetOfBits % 16;

        nrOfBitsInFistByte  = 16 - offsetOfStartBit;
        nrOfBitsInFistByte  = (nrOfBitsInFistByte > 14)?14:nrOfBitsInFistByte;
        nrOfLeftBits        = 14 - nrOfBitsInFistByte;

        //Copy last bits
        u14 = (pByteStream->bitsArray[indexOfUseBytes] >> offsetOfStartBit);
        u14 &= ~((0xFFFFu << nrOfBitsInFistByte));

        if (nrOfLeftBits > 0)
        {
            indexOfUseBytes++;
            if (indexOfUseBytes < BYTESTREAM16BIT_MAX)
            {
                tmp = 
                    pByteStream->bitsArray[indexOfUseBytes] & 
                    (~(0xFFFFu << nrOfLeftBits));
            }
            else
            {
                tmp = 0u;
            }
            tmp <<= nrOfBitsInFistByte;
        }

        u14 |= tmp;
    }
    else
    {
        printf("Error : pByteStream full!\n");
        u14 = 0u;
    }

    u14 = BIT_REVERSE_U14(u14);

    u14 &= BYTESTREAM14BIT_MASK;

    return u14;
}

unsigned short ByteStreamBit_GetU16(
        const struct ByteStreamBit* pByteStream,
        unsigned short index)
{
    unsigned short u16 = 0u;
    
    ARGUMENT_CHECK_NOT_NULL(pByteStream)

    if (index < BYTESTREAM16BIT_MAX)
    {
        u16 = pByteStream->bitsArray[index];
    }
    else
    {
        printf("Error : index overflow!\n");
        u16 = 0u;
    }

    u16 = BIT_REVERSE_U16(u16);

    u16 &= BYTESTREAM16BIT_MASK;

    return u16;
}

int ByteStreamBit_GetNrOfBits(const struct ByteStreamBit* pByteStream)
{
    ARGUMENT_CHECK_NOT_NULL(pByteStream)

    return (pByteStream->nrOfBits);
}

int ByteStreamBit_GetNrOfU14Bytes(const struct ByteStreamBit* pByteStream)
{
    ARGUMENT_CHECK_NOT_NULL(pByteStream)

    return (pByteStream->nrOfBits + 13) / 14;
}

int ByteStreamBit_GetNrOfU16Bytes(const struct ByteStreamBit* pByteStream)
{
    ARGUMENT_CHECK_NOT_NULL(pByteStream)

    return (pByteStream->nrOfBits + 15) / 16;
}

