#ifndef BYTEENCODEDECODE_H
#define BYTEENCODEDECODE_H

#define CPU_LE 0
#define CPU_BE 1
#define HOSTPC_ENDIAN CPU_LE
#define TARGET_ENDIAN CPU_BE

///////////////////////////////////////////////////////////////////////////////
//Global const variable and enum
#define BYTESTREAM14BIT_MAX     8
#define BYTESTREAM14BIT_MASK    0x3FFFu

#define BYTESTREAM16BIT_MAX     7
#define BYTESTREAM16BIT_MASK    0xFFFFu

struct ByteStreamBit;

typedef enum BSB_RESULT
{
    BSB_FALSE = 0,
    BSB_TRUE = 1
}BSB_RESULT;


///////////////////////////////////////////////////////////////////////////////
//Global interface declare
/**
 * @brief Create a new ByteStreamBit object.
 */
struct ByteStreamBit* 
ByteStreamBit_Init(void);

/**
 * @brief Copy pSrc Object to pDst Object.
 */
BSB_RESULT
ByteStreamBit_Copy(
        const struct ByteStreamBit* pSrc,
        struct ByteStreamBit* pDst
);

/**
 * @brief Zero ByteStreamBit Object.
 */
BSB_RESULT
ByteStreamBit_Zero(
        struct ByteStreamBit* pByteStream
);

/**
 * @brief Delete ByteStreamBit Object.
 */
BSB_RESULT
ByteStreamBit_Fini(
        struct ByteStreamBit* pByteStream
);

/**
 * @brief Append a unsigned 14 bit data to Object
 */
BSB_RESULT
ByteStreamBit_AppendU14(
        struct ByteStreamBit* pByteStream,
        unsigned short u14
);

/**
 * @brief Append a unsigned 16 bit data to Object
 */
BSB_RESULT
ByteStreamBit_AppendU16(
        struct ByteStreamBit* pByteStream, 
        unsigned short u16
);

/**
 * @brief Get unsigned 14 bit from index
 */
unsigned short
ByteStreamBit_GetU14(
        const struct ByteStreamBit* pByteStream,
        unsigned short index
);

/**
 * @brief Get unsigned 16 bit from index
 */
unsigned short
ByteStreamBit_GetU16(
        const struct ByteStreamBit* pByteStream,
        unsigned short index
);

/**
 * @brief Get number of used bits.
 */
int
ByteStreamBit_GetNrOfBits(
        const struct ByteStreamBit* pByteStream
);

/**
 * @brief Get number of used unsigned 14 bit
 * @details
 *      If a u14 unit not full, it still have 1 number.
 */
int
ByteStreamBit_GetNrOfU14Bytes(
        const struct ByteStreamBit* pByteStream
);

/**
 * @brief Get number of used unsigned 16 bit
 * @details
 *      If a u16 unit not full, it still have 1 number.
 */
int
ByteStreamBit_GetNrOfU16Bytes(
        const struct ByteStreamBit* pByteStream
);


#endif

