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
#include "pdbcommon.h"

#ifdef _MSC_VER
#define __func__ __FUNCTION__
#endif

///////////////////////////////////////////////////////////////////////////////
//Global helper function define
uint8_t fread_u8(FILE* fp)
{
    uint8_t u8 = 0u;
    fread(&u8, sizeof(uint8_t), 1, fp);
    return u8;
}

uint16_t fread_u16(FILE* fp)
{
    uint16_t u16 = 0u;
    fread(&u16, sizeof(uint8_t), 2, fp);
    return ENDIAN_BE2LE_U16(u16);
}

uint32_t fread_u32(FILE* fp)
{
    uint32_t u32 = 0u;
    fread(&u32, sizeof(uint8_t), 4, fp);
    return ENDIAN_BE2LE_U32(u32);
}

int fread_blob(void* p, int nrOfBytes, FILE* fp)
{
    return fread(p, sizeof(uint8_t), nrOfBytes, fp);
}

void printf_string(uint8_t* s, int len)
{
    while(len-- >0)
    {
        if (*s == '\n')
        {
            putchar('\\');
            putchar('n');
        }
        else if (*s == '\0')
        {
            break;
        }
        else
        {
            putchar(*s);
        }
        s++;
    }
}

void printf_hexstring(uint8_t* s, int len)
{
    while(len-- >0)
    {
        printf("%02x ",*s++);
    }
}

