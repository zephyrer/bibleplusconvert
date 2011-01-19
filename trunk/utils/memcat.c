#include "memcat.h"

void set_color(unsigned short color)
{
#ifdef __WINDOWS_32__
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
#else
    printf("\033[1;40;%dm",color);
#endif
}   

void memcat(void* memory,int size,const char* comment)
{
    char comment_buf[14] = "- -----------";
    char comment_len = 0;
    int i = 0;
    unsigned char* ptr = NULL;
    unsigned char  tmp = 0;
    unsigned char  lower = 0;
    unsigned char  upper = 0;

    if ((memory == NULL) || (comment == NULL))
    {
        printf("memory or comment is NULL!\nfunction return!\n");
#ifdef __WINDOWS_32__
        set_color(C_GREY);
#else
        printf("\033[0m");
#endif
        return;
    }

    if (size <= 0)
    {
        printf("memory size is zero!\n");
#ifdef __WINDOWS_32__
        set_color(C_GREY);
#else
        printf("\033[0m");
#endif
        return;
    }

    comment_len = strlen(comment);
    ptr = (unsigned char*)memory;

    if (comment_len > 9)
    {
        comment_len = 9;   
    }

    if (comment != NULL)
    {
        memcpy(comment_buf + 2,comment,comment_len);
        comment_buf[comment_len + 2] = ' ';
    }

    set_color(C_YELLOW);
    printf("%s",comment_buf);
    set_color(C_DARKER_GREY);
    printf("  00 01   02 03   04 05   06 07   08 09   0a 0b   0c 0d   0e 0f\n");

    for(i = 0; i < size;i++)
    {
        if (i % 16 == 0)
        {
            set_color(C_DARKER_GREY);
            printf("      0x%04x   ",i);
        }

        tmp = ptr[i];
        //printf("\n%c = %d ",tmp,tmp);
        lower = tmp & 0x0f;
        upper = (tmp & 0xf0) >> 4;
        if (lower >= 10)
        {
            lower = lower - 10 + 'a';
        }
        else 
        {
            lower = lower -  0 + '0';
        }
        
        if (upper >= 10)
        {
            upper = upper - 10 + 'a';
        }
        else 
        {
            upper = upper -  0 + '0';
        }

        set_color(C_LIGHT_GREEN);
        printf("%c%c",upper,lower);

        set_color(C_LIGHT_BLUE);
        if (i == size - 1)
        {
            printf("\n");
        }
        else if (i % 16 == 15)
        {
            printf("\n");
        }
        else if (i % 4 == 1)
        {
            printf(" , ");
        }
        else if (i % 4 == 3)
        {
            printf(" | ");
        }
        else 
        {
            printf(" ");
        }
    }

#ifdef __WINDOWS_32__
    set_color(C_GREY);
#else
    printf("\033[0m");
#endif
}

void memcat_with_ascii(void* memory,int size,const char* comment)
{
    char comment_buf[14] = "- -----------";
    char comment_len = 0;
    
    //Using for ascii output
    char ascii_line[17] = {0};
    char* ascii_ptr     = NULL;
    //print last char and space leave
    int ascii_leave     = 0;

    int i = 0;
    unsigned char* ptr = NULL;
    unsigned char  tmp = 0;
    unsigned char  lower = 0;
    unsigned char  upper = 0;

    if ((memory == NULL) || (comment == NULL))
    {
        printf("memory or comment is NULL!\nfunction return!\n");
#ifdef __WINDOWS_32__
        set_color(C_GREY);
#else
        printf("\033[0m");
#endif
        return;
    }

    if (size <= 0)
    {
        printf("memory size is zero!\n");
#ifdef __WINDOWS_32__
        set_color(C_GREY);
#else
        printf("\033[0m");
#endif
        return;
    }

    comment_len = strlen(comment);
    ptr = (unsigned char*)memory;

    if (comment_len > 9)
    {
        comment_len = 9;   
    }

    if (comment != NULL)
    {
        memcpy(comment_buf + 2,comment,comment_len);
        comment_buf[comment_len + 2] = ' ';
    }
    set_color(C_YELLOW);
    printf("%s",comment_buf);
    set_color(C_DARKER_GREY);
    printf("  00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n");

    
    for(i = 0; i < size;i++)
    {
        if (i % 16 == 0)
        {
            set_color(C_DARKER_GREY);
            memset(ascii_line,0,17);
            ascii_ptr = ascii_line;
            printf("      0x%04x   ",i);
        }
        
        tmp = ptr[i];
        //printf("\n%c = %d ",tmp,tmp);
        lower = tmp & 0x0f;
        upper = (tmp & 0xf0) >> 4;
        if (lower >= 10)
        {
            lower = lower - 10 + 'a';
        }
        else 
        {
            lower = lower -  0 + '0';
        }
        
        if (upper >= 10)
        {
            upper = upper - 10 + 'a';
        }
        else 
        {
            upper = upper -  0 + '0';
        }

        set_color(C_LIGHT_GREEN);
        printf("%c%c",upper,lower);

        //if (isgraph(tmp) || (tmp == ' '))
        if ((31 < tmp) && (tmp < 127))
        {
            *ascii_ptr++ = tmp;
        }
        else 
        {
            *ascii_ptr++ = '.';
        }

        set_color(C_LIGHT_BLUE);
        if (i == size - 1)
        {
            ascii_leave = 15 - i % 16;
            while(ascii_leave-- > 0)
            {
                printf("   ");
            }
            set_color(C_WHITE);
            printf(" %s",ascii_line);
            printf("\n");
        }
        else if (i % 16 == 15)
        {
            set_color(C_WHITE);
            printf(" %s",ascii_line);
            printf("\n");
        }
        else if (i % 4 == 1)
        {
            printf(",");
        }
        else if (i % 4 == 3)
        {
            printf("|");
        }
        else 
        {
            printf(" ");
        }
    }

#ifdef __WINDOWS_32__
    set_color(C_GREY);
#else
    printf("\033[0m");
#endif
}
