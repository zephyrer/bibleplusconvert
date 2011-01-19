//Generate by python.
/**
 * @file
 * @brief
 *
 * @details 
 * @author  
 * @date    
 * @version 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "byteUtils.h"

/**
 * @brief program entry.
 * @details
 *        The first function call by systm.
 * @param[in]   argc number of command line arguments.
 * @param[in]   argv array of command line arguments.
 * @return      any int value.
 */
int main(int argc, const char* argv[])
{
    struct ByteStreamBit* pByteStream;
    int i = 0;
    unsigned int tmp = 0;

    ///////////////////////////////////////////////////////
    //Try to encode u14.
    printf(" * Encode data......\n");

    //Initial
    pByteStream = ByteStreamBit_Init();

    //Init data.
    ByteStreamBit_AppendU14(pByteStream, 0xFFFF);
    ByteStreamBit_AppendU14(pByteStream, 0x0017);
    ByteStreamBit_AppendU14(pByteStream, 0x0032);
    ByteStreamBit_AppendU14(pByteStream, 0x0021);
    ByteStreamBit_AppendU14(pByteStream, 0x0010);
    ByteStreamBit_AppendU14(pByteStream, 0x0033);
    ByteStreamBit_AppendU14(pByteStream, 0x0001);
    ByteStreamBit_AppendU14(pByteStream, 0x0045);

    //Read back
    for (i = 0;i < BYTESTREAM14BIT_MAX;i++)
    {
        tmp = ByteStreamBit_GetU14(pByteStream, i);
        printf("U14[%d] = 0x%04x\n", i,tmp);
    }

    printf("\n");

    for (i = 0;i < BYTESTREAM16BIT_MAX;i++)
    {
        tmp = ByteStreamBit_GetU16(pByteStream, i);
        printf("U16[%d] = 0x%04x\n", i,tmp);
    }

    free(pByteStream);
    pByteStream = NULL;
    printf("\n");

    ///////////////////////////////////////////////////////
    //Try to decode u16
    printf(" * Decode data......\n");

    //Initial
    pByteStream = ByteStreamBit_Init();

    //Init data.
    ByteStreamBit_AppendU16(pByteStream, 0xFFFC);
    ByteStreamBit_AppendU16(pByteStream, 0x0170);
    ByteStreamBit_AppendU16(pByteStream, 0x0C80);
    ByteStreamBit_AppendU16(pByteStream, 0x2100);
    ByteStreamBit_AppendU16(pByteStream, 0x4003);
    ByteStreamBit_AppendU16(pByteStream, 0x3000);
    ByteStreamBit_AppendU16(pByteStream, 0x4045);

    //Read back
    for (i = 0;i < BYTESTREAM16BIT_MAX;i++)
    {
        tmp = ByteStreamBit_GetU16(pByteStream, i);
        printf("U16[%d] = 0x%04x\n", i,tmp);
    }

    printf("\n");

    for (i = 0;i < BYTESTREAM14BIT_MAX;i++)
    {
        tmp = ByteStreamBit_GetU14(pByteStream, i);
        printf("U14[%d] = 0x%04x\n", i,tmp);
    }

    free(pByteStream);
    pByteStream = NULL;
    printf("\n");

    return 0;
}

