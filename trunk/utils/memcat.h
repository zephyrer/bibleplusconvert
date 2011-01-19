#ifndef MEMCAT_H
#define MEMCAT_H
#include <stdio.h>
#include <string.h>

/*
*         *         *         *         *         *         *         *         
01234567890123456789012345678901234567890123456789012345678901234567890123456789

comment_comm   00 01   02 03   04 05   06 07   08 09   0a 0b   0c 0d   0e 0f
      0x0000   00 00 , 00 00 | 00 00 , 00 00 | 00 00 , 00 00 | 00 00 , 00 00
      0x0010   01 23 , 3a 34 | 98 00 , d0 ff | f1 f1 , ed cd | a4 3c , 
      0x0020  

*/

//--------------------------
#ifdef _MSC_VER

#ifndef __WINDOWS_32__
#define __WINDOWS_32__
#endif

#endif
//-----------------------
#ifdef __MINGW_H

#ifndef __WINDOWS_32__
#define __WINDOWS_32__
#endif

#endif
//--------------------------
#ifdef __WINDOWS_32__
#include <windows.h>

enum ColorsEnum
{
	C_BLACK        = 0,
	C_BLUE         = 1,
	C_DARK_GREEN   = 2,
	C_LIGHT_BLUE   = 3,
	C_RED          = 4,
	C_PURPLE       = 5,
	C_ORANGE       = 6,
	C_GREY         = 7,
	C_DARKER_GREY  = 8,
	C_MEDIUM_BLUE  = 9,
	C_LIGHT_GREEN  = 10,
	C_TEAL         = 11,
	C_RED_ORANGE   = 12,
	C_LIGHT_PURPLE = 13,
	C_YELLOW       = 14,
	C_WHITE        = 15,
};
#else
//Only Front color
enum ColorsEnum
{
	C_BLACK        = 30,
	C_RED          = 31,
	C_LIGHT_GREEN  = 32,
    C_YELLOW       = 33,
	C_BLUE         = 34,
	C_PURPLE       = 35,
	C_MEDIUM_BLUE  = 36,
	C_WHITE        = 37,

	C_DARK_GREEN   = 32,
	C_LIGHT_BLUE   = 34,
	C_ORANGE       = 33,
	C_GREY         = 30,
	C_DARKER_GREY  = 37,
	C_TEAL         = 30,
	C_RED_ORANGE   = 33,
	C_LIGHT_PURPLE = 31,
};
#endif

void set_color(unsigned short color);

void memcat(void* memory,int size,const char* comment);

void memcat_with_ascii(void* memory,int size,const char* comment);
#endif

