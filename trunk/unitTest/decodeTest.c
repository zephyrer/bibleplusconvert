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
#include "pdbdecoder.h"
#include "pdbhelper.h"
#include "byteUtils.h"
#include "memcat.h"

//#define TEST_DEBUG

#define ENDIAN_BE2LE_U16(u16) \
    ((((u16) << 8) & 0xFF00u) | (((u16) >> 8) & 0x00FFu))

#define maskTextType 0x3FFF
#define bookTextType 0xFFFF
#define chapTextType 0xFFFE
#define descTextType 0xFFFD
#define versTextType 0xFFFC

void PrintChar(const uint8_t* s, int size)
{
    int i = 0;
    for (i = 0;i < size;i++)
    {
        putchar(s[i]);
    }
}

void DecompressWord(
        const struct PDBLayout* pdbLayout,
        uint16_t wordIndex,
        FILE* fp)
{
    struct WordOffset  wordOffset;
    uint8_t  wordBuffer[128];
    uint8_t  bDescriptData = 0u;
    int i = 0;

    //set_color(C_YELLOW);
    //printf("##0x%04x##", wordIndex);
    //set_color(C_GREY);

    //先判断是否是控制信息
    switch(wordIndex & maskTextType)
    {
        case 0x0000:
            bDescriptData = 1u;
            break;
        case (bookTextType & maskTextType):
            printf("<BOOKTEXT>");
            bDescriptData = 1u;
            break;
        case (chapTextType & maskTextType):
            printf("<CHAPTEXT>");
            bDescriptData = 1u;
            break;
        case (descTextType & maskTextType):
            printf("<DESCTEXT>");
            bDescriptData = 1u;
            break;
        case (versTextType & maskTextType):
            printf("<VERSTEXT>");
            bDescriptData = 1u;
            break;
        default:
            bDescriptData = 0u;
            break;
    }

    if (bDescriptData)
    {
        return;
    }

    //获取字索引对应的数据偏移
    wordOffset = GetWordOffsetFromPDB(pdbLayout, wordIndex);

    if (wordOffset.offset > 0)
    {
        //读取字的索引对应的数据
        JumpToOffset(wordOffset.offset, fp);
        memset(wordBuffer, 0, sizeof(wordBuffer));

        fread(wordBuffer, sizeof(uint8_t), wordOffset.nrOfBytes, fp);

        //判断是否需要再次解码
        if (!wordOffset.boolCompressed)
        {
            PrintChar(wordBuffer, wordOffset.nrOfBytes);
        }
        else
        {
            //printf("\n");
            //printf("------sta------\n");
            //memcat(wordBuffer, wordOffset.nrOfBytes, "");
            //递归解码
            for (i = 0;i < (wordOffset.nrOfBytes / 2);i++)
            {
                wordIndex  = (wordBuffer[2 * i + 1]) & 0x00FFu;
                wordIndex |= (wordBuffer[2 * i + 0] << 8) & 0xFF00u;
                //printf("\n");
                //printf("## (%02x %02x) ##\n", wordBuffer[0], wordBuffer[1]);
                //printf("## (%02x %02x) ##\n", wordBuffer[2], wordBuffer[3]);
                //wordIndex  = ENDIAN_BE2LE_U16(wordIndex);

                DecompressWord(pdbLayout, wordIndex, fp);
            }
            //printf("\n");
            //printf("------end------\n");
        }
        if (pdbLayout->version.sepChar > 0)
        {
            printf(" ");
        }
    }
    else
    {
        printf("<## 0x%04x Not found##> ", wordIndex);
    }
}


void DecompressVerse(
        const struct PDBLayout* pdbLayout,
        const uint16_t* verseData, 
        const uint32_t  verseNROfWords,
        uint32_t verseOffset,
        uint16_t nrOfWords,
        FILE* fp)
{
    uint16_t wordIndex = 0u;
    int i = 0;

    for (i = 0;i < nrOfWords;i++)
    {
        if (verseOffset + i >= verseNROfWords)
        {
            set_color(C_RED_ORANGE);
            printf("Overflow!!!\n");
            set_color(C_GREY);
            break;
        }

        wordIndex = (verseData + verseOffset)[i];
        DecompressWord(pdbLayout, wordIndex, fp);
    }
}


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
    FILE* fp;
    struct PDBLayout* pdbLayout = NULL;
    struct VerseOffset verseOffset;
    struct VerseData verseData;

    uint16_t bookToView   = 0u;
    uint16_t chapterToView= 0u;
    uint32_t verseToView  = 0u;

    uint32_t nrOfVerses   = 0u;

    int i = 0;
    int inputdata = 0;

    if (argc == 2)
    {
        fp = fopen(argv[1], "rb");
        if (fp == NULL)
        {
            perror("open pdb file fail");
            return -1;
        }
    }
    else
    {
        printf("need input file\n");
        return -2;
    }

    pdbLayout = DecodePDBFile(fp);

    if (pdbLayout != NULL)
    {
        //PDBPrintBookInformation(pdbLayout);
        //PDBPrintWordDict(pdbLayout, fp);
        //
        //不成功的：
        //约珥书(29)
        //阿摩司书（30）5章及以后，返回的verse字节个数错误
        //
        bookToView      = 30;
        chapterToView   = 7;

        printf("bookToView :");
        scanf("%d", &inputdata);
        bookToView = inputdata;

        printf("chapterToView :");
        scanf("%d", &inputdata);
        chapterToView = inputdata;

        printf("%d,%d\n", bookToView, chapterToView);

        //Print book chapter and verse info
        PDBPrintOneBooKInfo(pdbLayout, bookToView);
        nrOfVerses = GetNumberOfVerses(pdbLayout,bookToView,i);

        //Decompress book data.
        verseData = DecompressBookAllVerseData(pdbLayout,bookToView,fp);

        //Decode word index data to text data.
        if ((verseData.verseBuffer != NULL) &&
            (verseData.nrOfWords > 0))
        {
            /*
            memcat_with_ascii(
                    verseData.verseBuffer, 
                    verseData.nrOfWords * 2,
                    "decompressed");
            */        
            for (i = 1;i <= nrOfVerses;i++)
            {
                //准备读取i
                verseToView = i;

                //尝试解码一段文字
                verseOffset = GetVerseOffsetFromPDB(
                        pdbLayout,
                        bookToView,
                        chapterToView,
                        verseToView);
#ifdef TEST_DEBUG
                printf("verseOffset.offset 0x%08lx\n", verseOffset.offset);
                printf("verseOffset.nrOfWords %d\n", verseOffset.nrOfWords);
#endif

                set_color(C_DARKER_GREY);
                if (verseOffset.offset != VERSEOFFSET_INVAILD)
                {
                    printf("<VERSE>");
                    DecompressVerse(
                            pdbLayout,
                            verseData.verseBuffer,
                            verseData.nrOfWords,
                            verseOffset.offset,
                            verseOffset.nrOfWords,
                            fp);
                    printf("</VERSE>");
                    printf("\n");
                }
                set_color(C_GREY);
            }

            //释放解码的数据
            free(verseData.verseBuffer);
            verseData.verseBuffer = NULL;
        }

        //释放PDBLayout
        FreePDBFile(pdbLayout);
        pdbLayout = NULL;
    }

    //关闭输入文件
    fclose(fp);
    fp = NULL;

    return 0;
}

