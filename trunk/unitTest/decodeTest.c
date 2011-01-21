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
#include "dictUtils.h"
#include "memcat.h"

//#define TEST_DEBUG


void PrintChar(const uint8_t* s, int size)
{
    int i = 0;
    for (i = 0;i < size;i++)
    {
        putchar(s[i]);
    }
}


#define MAX_BUFFER_SIZE 4096
static uint8_t buffer[MAX_BUFFER_SIZE];

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
    struct WordBook*  pWordBook = NULL;

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
        //Optimize word.
        pWordBook = BuildWordDictInRam(pdbLayout, fp);
        if (pWordBook == NULL)
        {
            printf("Error build word dict!\n");
            FreePDBFile(pdbLayout);
            pdbLayout = NULL;
        }
    }

    if (pdbLayout != NULL)
    {
        bookToView      = 30;
        chapterToView   = 7;

        printf("bookToView :");
        scanf("%d", &inputdata);
        bookToView = inputdata;

        printf("chapterToView :");
        scanf("%d", &inputdata);
        chapterToView = inputdata;

        printf("to view book : %d, chapter : %d\n", bookToView, chapterToView);

        //Print book chapter and verse info
        //PDBPrintOneBooKInfo(pdbLayout, bookToView);
        nrOfVerses = GetNumberOfVerses(pdbLayout,bookToView,chapterToView);

        //Decompress book data.
        verseData = DecompressBookAllVerseData(pdbLayout,bookToView,fp);

        //Decode word index data to text data.
        if ((verseData.verseBuffer != NULL) &&
            (verseData.nrOfWords > 0))
        {   
            for (i = 1;i <= nrOfVerses;i++)
            {
                verseToView = i;

                memset(buffer, 0, MAX_BUFFER_SIZE);

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
                            pWordBook,
                            fp,
                            buffer);
                    printf("%s",buffer);
                    printf("</VERSE>");
                    printf("\n");
                }
                set_color(C_GREY);
            }

            //release verse buffer.
            free(verseData.verseBuffer);
            verseData.verseBuffer = NULL;
        }

        //Release word book
        WordBook_fini(pWordBook);
        pWordBook = NULL;

        //release pdb struct.
        FreePDBFile(pdbLayout);
        pdbLayout = NULL;
    }

    //close input file
    fclose(fp);
    fp = NULL;

    return 0;
}

