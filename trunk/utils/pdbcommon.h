#ifndef PDBCOMMON_H
#define PDBCOMMON_H

///////////////////////////////////////////////////////////////////////////////
//Global variable type define
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long  uint32_t;

///////////////////////////////////////////////////////////////////////////////
//Global macor define
#define MSG_MALLOC_FAIL(p) \
    printf("%s is NULL, malloc fail at %s:%d\n", #p,__FILE__,__LINE__);

#define ENDIAN_BE2LE_U16(u16) \
    ((((u16) << 8) & 0xFF00u) | (((u16) >> 8) & 0x00FFu))

#define ENDIAN_BE2LE_U32(u32) \
    ((((u32) << 24) & 0xFF000000u) | \
     (((u32) <<  8) & 0x00FF0000u) | \
     (((u32) >>  8) & 0x0000FF00u) | \
     (((u32) >> 24) & 0x000000FFu))

#define ENDIAN_LE2BE_U16 ENDIAN_BE2LE_U16
#define ENDIAN_LE2BE_U32 ENDIAN_BE2LE_U32

///////////////////////////////////////////////////////////////////////////////
//Global helper function declare
uint8_t fread_u8(FILE* fp);
uint16_t fread_u16(FILE* fp);
uint32_t fread_u32(FILE* fp);

int fread_blob(void* p, int nrOfBytes, FILE* fp);
void printf_string(uint8_t* s, int len);
void printf_hexstring(uint8_t* s, int len);

#endif

