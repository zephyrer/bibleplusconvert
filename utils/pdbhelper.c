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
#include <string.h>
#include "byteUtils.h"
#include "dictUtils.h"
#include "pdbhelper.h"

///////////////////////////////////////////////////////////////////////////////
//Local interface declare
///decode 14bit/16bit encoded data.
static uint16_t DecompressVerseData(
        const uint16_t* compressedVerseData,
        uint32_t nrOfDataWords,
        uint16_t* decompressedVerseData,
        uint32_t nrOfDecompressedDataBytes
        );

///Decode 
static int DecompressWord(
        const struct PDBLayout* pdbLayout,
        uint16_t wordIndex,
        const struct WordBook* pWordBook,
        FILE* fp,
        uint8_t* buffer);

#define ENDIAN_BE2LE_U16(u16) \
    ((((u16) << 8) & 0xFF00u) | (((u16) >> 8) & 0x00FFu))

#define maskTextType 0x3FFF
#define bookTextType 0xFFFF
#define chapTextType 0xFFFE
#define descTextType 0xFFFD
#define versTextType 0xFFFC


///////////////////////////////////////////////////////////////////////////////
//Global interface define
uint16_t GetNumberOfChapters(
        const struct PDBLayout* pdbLayout,
        uint8_t  bookToView)//From 1
{
    const struct RecordBookDetail* bookDetail = NULL;

    if ((0 < bookToView) && (bookToView <= pdbLayout->version.totalBooks))
    {
        bookDetail = &(pdbLayout->bookDetail[bookToView - 1]);
        return bookDetail->bookDetailInfo.numChapters;
    }
    else
    {
        printf("bookToView is invaild!\n");
        return 0;
    }
}

uint16_t GetNumberOfVerses(
        const struct PDBLayout* pdbLayout,
        uint8_t  bookToView,//From 1
        uint8_t  chapterToView)//From 1
{
    const struct RecordBookDetail* bookDetail = NULL;
    const uint16_t* lastVerseNumber = NULL;

    if ((0 < bookToView) && (bookToView <= pdbLayout->version.totalBooks))
    {
        bookDetail = &(pdbLayout->bookDetail[bookToView - 1]);
        lastVerseNumber = bookDetail->bookDetailInfo.lastVerseNumber;

        if(chapterToView <= bookDetail->bookDetailInfo.numChapters)
        {
            if (chapterToView > 1)
            {
                return lastVerseNumber[(chapterToView - 1)] -
                    lastVerseNumber[(chapterToView - 1) - 1];
            }
            else
            {
                return lastVerseNumber[0];
            }
        }
        else
        {
            printf("chapterToView is invaild!\n");
            return 0;
        }
    }
    else
    {
        printf("bookToView is invaild!\n");
        return 0;
    }
}

struct VerseOffset GetVerseOffsetFromPDB(
        const struct PDBLayout* pdbLayout,
        uint8_t  bookToView,
        uint8_t  chapterToView,
        uint16_t verseToView)
{
    const struct RecordBookDetail* bookDetail = NULL;
    const uint16_t* lastVerseNumber = NULL;
    const uint32_t* chapterOffset = NULL;
    const uint16_t* verseOffset = NULL;

    uint16_t nrOfWords = 0;
    uint16_t nchaptersFirstVerse = 0u;
    uint32_t nverseOffset = 0u;

    struct VerseOffset ret_value;
    ret_value.offset = VERSEOFFSET_INVAILD;
    ret_value.nrOfWords = 0;

#ifdef PDB_DEBUG
    printf("bookToView    %d\n", bookToView);
    printf("chapterToView %d\n", chapterToView);
    printf("verseToView   %d\n", verseToView);
#endif

    if ((0 < bookToView) && (bookToView <= pdbLayout->version.totalBooks))
    {
        bookDetail = &(pdbLayout->bookDetail[bookToView - 1]);
        lastVerseNumber = bookDetail->bookDetailInfo.lastVerseNumber;
        chapterOffset   = bookDetail->bookDetailInfo.chapterOffset;
        verseOffset     = bookDetail->bookDetailInfo.verseOffset;
    }
    else
    {
        printf("bookToView is invaild!\n");
        return ret_value;
    }

    //the first Verse of a chapter.
    //because lastVerseNumber is the number which
    //start with 1, add up each number verse of chapter before
    //in same book.
    //so the number is the fist verse of next chapter.
    nchaptersFirstVerse = 0; 

    if (chapterToView > 1)
    {
        nchaptersFirstVerse = lastVerseNumber[chapterToView - 2];
    }

    nverseOffset = chapterOffset[chapterToView - 1];

    //Get the offset os a verse.
    if (verseToView > 1)
    {
        nverseOffset += 
            verseOffset[nchaptersFirstVerse + verseToView - 2];
    }

    //Caclute the number of words in this verse
    //Each word use 2 bytes.
    if (verseToView > 1) 
    { 
        nrOfWords = (
            verseOffset[nchaptersFirstVerse + verseToView - 1] -
            verseOffset[nchaptersFirstVerse + verseToView - 2]);
    }
    else
    { 
        nrOfWords = verseOffset[nchaptersFirstVerse];
    } 

#ifdef PDB_DEBUG
    printf("nchaptersFirstVerse %d\n", nchaptersFirstVerse);

    {
        int i = 0;
        for (i = 0;i <= nchaptersFirstVerse;i++)
        {
            printf("verseOffset[%03d] = 0x%08x\n",i,verseOffset[i]);
        }
    }
    
    printf("nrOfWords %d\n", nrOfWords);
#endif

    //make sure verse is not more than 1024.
    //if more than 1024, it must be error.
    nrOfWords &= 0x3FF;

    //Save result
    ret_value.offset = nverseOffset;
    ret_value.nrOfWords = nrOfWords;

    return ret_value;
}

struct WordOffset GetWordOffsetFromPDB(
        const struct PDBLayout* pdbLayout,
        uint16_t wordIndex)
{
    uint16_t nrOfWordTable = 0u;
    uint32_t wordDictOffset = 0u;
    const struct WordBriefData* wordBriefData = NULL;
    int i = 0;
    ///the index of dict which contain wordIndex 
    int indexOfDict = 0;
    ///the number of word befor of this index.
    uint16_t preIndex = 0u;
    ///index of word in this dict.
    uint16_t currIndex = 0u;
    ///word data offset in file
    uint32_t wordOffset = 0u;

    struct WordOffset ret_value;

    ret_value.offset = 0u;
    ret_value.nrOfBytes = 0u;
    ret_value.boolCompressed = 0u;

    //save some data from local access
    nrOfWordTable = pdbLayout->wordTable.totalIndexes;
    wordBriefData = pdbLayout->wordTable.wordBriefData;
    wordDictOffset = pdbLayout->wordData.offset;

    //decide wordIndex belong to which dict, index of this dict.
    for (i = 0;i < nrOfWordTable;i++)
    {
        if ((wordIndex - 1) < (preIndex + wordBriefData[i].totalWord))
        {
            indexOfDict = i;
            currIndex = (wordIndex - 1) - preIndex;
            break;
        }
        else
        {
            preIndex += wordBriefData[i].totalWord;
        }
    }

    if (i == nrOfWordTable)
    {
#ifdef PDB_DEBUG
        printf("##wordIndex not found in dict!\n");
#endif
        return ret_value;
    }

    //Cacluate total number of bytes before this dict.
    for (i = 0;i < indexOfDict;i++)
    {
        wordOffset += wordBriefData[i].totalWord * wordBriefData[i].wordLength;
    }

    //cacluate number of bytes before current word.
    wordOffset += wordBriefData[indexOfDict].wordLength * currIndex;

    //change offset of dict to offset of file.
    wordOffset += pdbLayout->wordData.offset;

    //save data.
    ret_value.offset = wordOffset;
    ret_value.nrOfBytes = wordBriefData[indexOfDict].wordLength;
    ret_value.boolCompressed = wordBriefData[indexOfDict].boolCompressed;

    return ret_value;
}

uint16_t PDBPrintOneBooKInfo(
        const struct PDBLayout* pdbLayout,
        uint16_t bookToView)
{
    uint16_t nrOfChapters = 0u;
    uint32_t nrOfVerses   = 0u;
    int i = 0;

    nrOfChapters = GetNumberOfChapters(pdbLayout,bookToView);
    printf("book %s have %d chapters\n",
            pdbLayout->version.bookBriefInfo[bookToView - 1].simpleName,
            nrOfChapters);
    for (i = 1;i <= nrOfChapters;i++)
    {
        nrOfVerses = GetNumberOfVerses(pdbLayout,bookToView,i);
        printf("  chapter %d have %ld verse\n", i, nrOfVerses);
    }

    return 0;
}

uint16_t PDBPrintAllBookInfo(
        const struct PDBLayout* pdbLayout)
{
    int i = 0;

    for (i = 1;i <= pdbLayout->version.totalBooks;i++)
    {
        PDBPrintOneBooKInfo(pdbLayout, i);
        printf("\n");
    }

    return 0;
}

uint16_t PDBPrintWordDict(
        const struct PDBLayout* pdbLayout,
        FILE* fp)
{
    uint16_t nrOfWordTable = 0u;
    uint32_t wordDictOffset = 0u;
    const struct WordBriefData* wordBriefData = NULL;
    int i = 0;
    int j = 0;
    uint16_t totalWord = 0u;
    uint16_t wordLength = 0u;
    uint8_t  wordBuffer[128];
    uint32_t wordIndex = 1u;

    nrOfWordTable = pdbLayout->wordTable.totalIndexes;
    wordBriefData = pdbLayout->wordTable.wordBriefData;
    wordDictOffset = pdbLayout->wordData.offset;

    JumpToOffset(wordDictOffset, fp);

    for (i = 0;i < nrOfWordTable;i++)
    {
        wordLength = wordBriefData[i].wordLength;
        totalWord  = wordBriefData[i].totalWord;
        printf("--wordLength %d, totalWord %d--\n",
                wordLength, totalWord);

        if (wordBriefData[i].boolCompressed)
        {
            printf("##compress data, big endian hex data.##\n");
            printf("## MSB ...  LSB ##\n");

            for (j = 0;j < totalWord;j++)
            {
                printf("[%08lx] ", wordIndex);
                wordIndex++;
                fread_blob(wordBuffer,wordLength,fp);
                printf_hexstring(wordBuffer,wordLength);
                printf("\n");
            }
        }
        else
        {

            for (j = 0;j < totalWord;j++)
            {
                printf("[%08lx] ", wordIndex);
                wordIndex++;
                fread_blob(wordBuffer,wordLength,fp);
                printf_string(wordBuffer,wordLength);
                printf("\n");
            }
        }
        printf("\n");
    }

    return 0;
}

struct VerseData DecompressBookAllVerseData(
        const struct PDBLayout* pdbLayout,
        uint8_t  bookToView,
        FILE* fp)
{
    //Book record text data start offset of file.
    uint32_t dataBaseOffset = 0u;
    //number of bytes in file(encoded)
    uint32_t nrOfDataBytes = 0u;
    //number of words in file(equ to nrOfDataBytes/2)
    uint32_t nrOfDataWords = 0u;
    //number of bytes have decoded.
    uint32_t nrOfDecompressedDataBytes = 0u;

    //end offset of pdb file.
    uint32_t fileEndOffset = 0u;
    //the first offset of book data index.
    uint16_t  bookDataIndex = 0u;
    //next book detail struct record index
    uint16_t  bookNextIndex  = 0u;

    //dynmaic data.
    uint16_t* compressedVerseData = NULL;
    uint16_t* decompressedVerseData = NULL;

    int i = 0;

    struct VerseData ret_value;

    ret_value.verseBuffer = NULL;
    ret_value.nrOfWords   = 0u;

    //Get the max offset fo file.
    fseek(fp,0,SEEK_END);
    fileEndOffset = ftell(fp);

    //If this is no the last book, then use next book details record offset minue
    //this book first data record offset will get total data length.
    //
    //Sample fragment of pdb layout is:
    //  Record detail(chapter/verse)
    //  Record text data
    //  Record text data
    //  Record text data
    //  Record text data
    //
    //  Record detail(chapter/verse)
    //  Record text data
    //  Record text data
    //
    dataBaseOffset = pdbLayout->bookDetail[bookToView - 1].bookDetailData.offset;

    if (bookToView > pdbLayout->version.totalBooks)
    {
#ifdef PDB_DEBUG
        printf("bookToView is invaild\n");
#endif
        nrOfDataBytes = 0u;
    }
    else  if (bookToView < pdbLayout->version.totalBooks)
    {
        bookDataIndex = pdbLayout->version.bookBriefInfo[bookToView - 1].bookIndex + 1;
        bookNextIndex = pdbLayout->version.bookBriefInfo[bookToView].bookIndex;
        
        nrOfDataBytes =
            pdbLayout->header.jumpTable[bookNextIndex].offset -
            pdbLayout->header.jumpTable[bookDataIndex].offset;
    }
    else if (bookToView == pdbLayout->version.totalBooks)
    {
        //if to view last book, use max file offset to get the data length
        bookDataIndex = pdbLayout->version.bookBriefInfo[bookToView - 1].bookIndex + 1;
        nrOfDataBytes = 
            fileEndOffset - 
            pdbLayout->header.jumpTable[bookDataIndex].offset;
    }

    //Malloc data.
    if (nrOfDataBytes > 0)
    {
        compressedVerseData = 
            (uint16_t*)malloc(nrOfDataBytes);
        if (compressedVerseData == NULL)
        {
            MSG_MALLOC_FAIL(compressedVerseData);
            nrOfDataBytes = 0u;
        }
        else
        {
            //Decode buffer.
            nrOfDecompressedDataBytes = (nrOfDataBytes * 8 + 6) / 7;
            decompressedVerseData = 
                (uint16_t*)malloc(nrOfDecompressedDataBytes * 2);

            if (decompressedVerseData == NULL)
            {
                free(compressedVerseData);
                compressedVerseData = NULL;

                MSG_MALLOC_FAIL(decompressedVerseData);
                nrOfDataBytes = 0u;
            }
        }
    }

    //Some debug information
#ifdef PDB_DEBUG
    printf("Book[%d].nrOfDataBytes = %ld\n",
            (bookToView - 1), nrOfDataBytes);
    printf("Book[%d].nrOfDecompressedDataBytes = %ld\n",
            (bookToView - 1), nrOfDecompressedDataBytes);
#endif

    //read data, and change endian inside.
    if ((nrOfDataBytes > 0) && 
        (compressedVerseData != NULL) && 
        (decompressedVerseData != NULL))
    {
        //Jump to file and readback real word data.
        fseek(fp, dataBaseOffset, 0);
        nrOfDataWords = nrOfDataBytes / 2;
        for (i = 0;i < nrOfDataWords;i++)
        {
            compressedVerseData[i] = fread_u16(fp);
        }

        //Decode data.
        DecompressVerseData(compressedVerseData, nrOfDataWords,
                decompressedVerseData, nrOfDecompressedDataBytes);

        free(compressedVerseData);
        compressedVerseData = NULL;

        //Save data.
        ret_value.verseBuffer = decompressedVerseData;
        ret_value.nrOfWords   = (nrOfDecompressedDataBytes + 1) / 2;
    }

    return ret_value;
}

void DecompressVerse(
        const struct PDBLayout* pdbLayout,
        const uint16_t* verseData, 
        const uint32_t  verseNROfWords,
        uint32_t verseOffset,
        uint16_t nrOfWords,
        const struct WordBook* pWordBook,
        FILE* fp,
        uint8_t* buffer)
{
    uint16_t wordIndex = 0u;
    int i = 0;
    int tmp = 0;

    for (i = 0;i < nrOfWords;i++)
    {
        if (verseOffset + i >= verseNROfWords)
        {
            printf("Overflow!!!\n");
            break;
        }

        wordIndex = (verseData + verseOffset)[i];
        tmp = DecompressWord(pdbLayout, wordIndex, pWordBook, fp, buffer);
        buffer += tmp;
    }
}

struct WordBook* BuildWordDictInRam(
        const struct PDBLayout* pdbLayout,
        FILE* fp)
{
    char buffer[MAX_WORD_LENGTH];
    struct WordBook* wordBook = NULL;
    uint32_t nrOfTotalWords = 0;
    uint32_t nrOfWords = 0;
    uint16_t lengthOfWord = 0;
    uint16_t totalIndexes = 0;
    int i = 0;
    int j = 0;

    totalIndexes = pdbLayout->wordTable.totalIndexes;

    for (i = 0;i < totalIndexes;i++)
    {
        nrOfWords += pdbLayout->wordTable.wordBriefData[i].totalWord;
    }

    nrOfTotalWords = nrOfWords;

    wordBook = WordBook_init(nrOfTotalWords);

    if (wordBook == NULL)
    {
        printf("Error create word book!\n");
        return NULL;
    }
    
    JumpToOffset(pdbLayout->wordData.offset, fp);

    for (i = 0;i < totalIndexes;i++)
    {
        nrOfWords = pdbLayout->wordTable.wordBriefData[i].totalWord;
        lengthOfWord = pdbLayout->wordTable.wordBriefData[i].wordLength;
        if (pdbLayout->wordTable.wordBriefData[i].boolCompressed)
        {
            for (j = 0;j < nrOfWords;j++)
            {
                memset(buffer, 0, MAX_WORD_LENGTH);
                fread(buffer, sizeof(uint8_t), lengthOfWord, fp);
                WordBook_AppendBLOB(wordBook, buffer, lengthOfWord);
            }
        }
        else
        {
            for (j = 0;j < nrOfWords;j++)
            {
                memset(buffer, 0, MAX_WORD_LENGTH);
                fread(buffer, sizeof(uint8_t), lengthOfWord, fp);
                WordBook_AppendWord(wordBook, buffer);
            }
        }   
    }

    return wordBook;
}

///////////////////////////////////////////////////////////////////////////////
//Local interface define
uint16_t DecompressVerseData(
        const uint16_t* compressedVerseData,
        uint32_t nrOfDataWords,
        uint16_t* decompressedVerseData,
        uint32_t nrOfDecompressedDataBytes
        )
{
    int i = 0;
    int j = 0;
    int endOfDecompressedData = 0u;
    struct ByteStreamBit* pByteStream;
    uint16_t wordIndex = 0u;

    //Initial bytestream.
    pByteStream = ByteStreamBit_Init();

#ifdef PDB_DEBUG
    printf("compressed   : ");
#endif
    for (i = 0;i < nrOfDataWords;i++)
    {
        //save data have been encoded.
#ifdef PDB_DEBUG
        set_color(C_LIGHT_GREEN);
        printf("%04x ", compressedVerseData[i]);
#endif

        ByteStreamBit_AppendU16(pByteStream,compressedVerseData[i]);

#ifdef PDB_DEBUG
        set_color(C_GREY);
#endif

        //each 8 14bit data decode to 7 16bit data.
        if ((i % 7) == 6)
        {
            //some debug information
#ifdef PDB_DEBUG
            printf("\n");

            printf("decompressed : ");
            set_color(C_DARKER_GREY);
            for (j = 0;j < 8;j++)
            {
                wordIndex = ByteStreamBit_GetU14(pByteStream,j);
                printf("%04x ", wordIndex);
            }
            printf("\n");
            set_color(C_GREY);

            printf("compressed   : ");
#endif
            //Map word index to real word.
            for (j = 0;j < 8;j++)
            {
                wordIndex = ByteStreamBit_GetU14(pByteStream,j);
                decompressedVerseData[endOfDecompressedData++] = wordIndex;
                //printf("%04x ", wordIndex);
            }

            //Clean bytestream
            ByteStreamBit_Zero(pByteStream);
        }
    }

    //If data not multi of 8, left data need to decode 
    //below.
    if ((i % 7) != 0)
    {
#ifdef PDB_DEBUG
        printf("\n");

        printf("decompressed : ");
        set_color(C_DARKER_GREY);
        for (j = 0;j < ByteStreamBit_GetNrOfU14Bytes(pByteStream);j++)
        {
            wordIndex = ByteStreamBit_GetU14(pByteStream,j);
            printf("%04x ", wordIndex);
        }
        printf("\n");
        set_color(C_GREY);
#endif

        //map word index to real word.
        for (j = 0;j < ByteStreamBit_GetNrOfU14Bytes(pByteStream);j++)
        {
            wordIndex = ByteStreamBit_GetU14(pByteStream,j);
            decompressedVerseData[endOfDecompressedData++] = wordIndex;
        }
    }

    //Release bytestream.
    ByteStreamBit_Fini(pByteStream);
    pByteStream = NULL;

    return endOfDecompressedData;
}

int DecompressWord(
        const struct PDBLayout* pdbLayout,
        uint16_t wordIndex,
        const struct WordBook* pWordBook,
        FILE* fp,
        uint8_t* buffer)
{
    struct WordOffset  wordOffset;
    uint8_t  wordBuffer[128];
    uint8_t  bDescriptData = 0u;
    int i = 0;

    const char* pResult = NULL;

    int ret_value = 0;
    int tmp = 0;

    //Descide if the index is control data.
    switch(wordIndex & maskTextType)
    {
        case 0x0000:
            bDescriptData = 1u;
            ret_value     = 0;
            break;
        case (bookTextType & maskTextType):
            memcpy(buffer, "<BOOKTEXT>", 10);
            ret_value     = 10;
            bDescriptData = 1u;
            break;
        case (chapTextType & maskTextType):
            memcpy(buffer, "<CHAPTEXT>", 10);
            ret_value     = 10;
            bDescriptData = 1u;
            break;
        case (descTextType & maskTextType):
            memcpy(buffer, "<DESCTEXT>", 10);
            ret_value     = 10;
            bDescriptData = 1u;
            break;
        case (versTextType & maskTextType):
            memcpy(buffer, "<VERSTEXT>", 10);
            ret_value     = 10;
            bDescriptData = 1u;
            break;
        default:
            ret_value     = 0;
            bDescriptData = 0u;
            break;
    }

    if (bDescriptData)
    {
        return ret_value;
    }
    else
    {
    }


    //get the word data offset from pdb.
    wordOffset = GetWordOffsetFromPDB(pdbLayout, wordIndex);

    if (wordOffset.offset > 0)
    {
        //read data from pdb file.
        //JumpToOffset(wordOffset.offset, fp);
        memset(wordBuffer, 0, sizeof(wordBuffer));

        //fread(wordBuffer, sizeof(uint8_t), wordOffset.nrOfBytes, fp);
        if (WordBook_GetWord(
            pWordBook,
            wordIndex - 1,
            &pResult) == WBK_OK)
        {
            memcpy(wordBuffer, pResult, wordOffset.nrOfBytes);
        }

        //decide whether need to decode again.
        if (!wordOffset.boolCompressed)
        {
            //PrintChar(wordBuffer, wordOffset.nrOfBytes);
            memcpy(buffer, wordBuffer, wordOffset.nrOfBytes);
            buffer    += wordOffset.nrOfBytes;
            ret_value += wordOffset.nrOfBytes;
        }
        else
        {
            //Decode data.
            for (i = 0;i < (wordOffset.nrOfBytes / 2);i++)
            {
                wordIndex  = (wordBuffer[2 * i + 1]) & 0x00FFu;
                wordIndex |= (wordBuffer[2 * i + 0] << 8) & 0xFF00u;

                tmp = DecompressWord(
                        pdbLayout,
                        wordIndex,
                        pWordBook,
                        fp,
                        buffer);
                buffer    += tmp;
                ret_value += tmp;
            }
        }
        if (pdbLayout->version.sepChar > 0)
        {
            //printf(" ");
            memcpy(buffer, " ", 1);
            buffer += 1;
            ret_value += 1;
        }
    }
    else
    {
        printf("<## 0x%04x Not found##> ", wordIndex);
    }

    return ret_value;
}


