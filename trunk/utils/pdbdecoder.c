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
#include <ctype.h>
#include "pdbdecoder.h"
#include "memcat.h"

//#define PDB_DEBUG


///////////////////////////////////////////////////////////////////////////////
//Local PDB decode interface declare
///解码PDB的文件头
static int DecodePDBHeader(struct PDBHeader* header, FILE* fp);
///解码PDB的跳转表
static int DecodePDBJumpTable(
        struct JumpTable* jumpTable, 
        uint16_t numRecs,
        FILE* fp);
///解码PDB的书卷摘要信息
static int DecodeBibleBookBriefInfo(
        struct BookBriefInfo* bookBriefInfo,
        int nrOfBooks,
        FILE* fp);
///解码PDB的书卷版本信息
static int DecodeBibleVersion(
        struct RecordVersion*    version,
        FILE* fp);
///解码PDB的单词表
static int DecodeBibleWordTable(
        struct RecordWordTable*  wordTable,
        FILE* fp);
///解码PDB的字典
static int DecodeBibleWordDict(
        struct RecordWordData* wordData,
        const struct RecordWordTable* wordTable,
        FILE* fp);
///解码PDB的书卷详细索引信息
static int DecodeBibleBookDetailInfo(
        struct BookDetailInfo* bookDetailInfo,
        FILE* fp);
///解码PDB的书卷数据
static int DecodeBibleBookDetailData(
        struct BookDetailData* bookDetailData,
        const struct BookDetailInfo* bookDetailInfo,
        FILE* fp);
///解码PDB的详细信息
static int DecodeBibleBookDetail(
        struct RecordBookDetail* bookDetail,
        const struct JumpTable*  jumpTable,
        const struct BookBriefInfo* bookBriefInfo,
        FILE* fp);

///////////////////////////////////////////////////////////////////////////////
//Local PDB decode interface define
int DecodePDBHeader(struct PDBHeader* header, FILE* fp)
{
    //Fill pdb header
    fread_blob(header->fileName, 32, fp);

    header->fileAttributes = fread_u16(fp);
    header->versionNumber = fread_u16(fp);

    header->createDate    = fread_u32(fp);
    header->modificationDate = fread_u32(fp);
    header->lastBackupDate = fread_u32(fp);

    header->modificationNumber = fread_u32(fp);

    header->appInfoArea   = fread_u32(fp);
    header->sortInfoArea  = fread_u32(fp);

    fread_blob(header->dataBaseType, 4, fp);
    fread_blob(header->creatorID, 4, fp);
    fread_blob(header->uniqueIDSeed, 4, fp);

    header->nextRecord    = fread_u32(fp);
    header->numRecs       = fread_u16(fp);

#ifdef PDB_DEBUG
    //Debug info
    printf("header->FileName    : %s\n", header->fileName);
    
    printf("header->DatabaseType: ");
        printf_string(header->dataBaseType, 4);
    printf("\n");
    
    printf("header->CreatorID   : ");
        printf_string(header->creatorID, 4);
    printf("\n");

    printf("header->NumRecs     : 0x%02x\n",header->numRecs);
#endif
    return 0;
}

int DecodePDBJumpTable(
        struct JumpTable* jumpTable, 
        uint16_t numRecs,
        FILE* fp)
{
    int i = 0;
    for (i = 0;i < numRecs;i++)
    {
        jumpTable[i].offset = fread_u32(fp);
        fread_blob(jumpTable[i].reserved, 4, fp);
    }

#ifdef PDB_DEBUG
    for (i = 0;i < numRecs;i++)
    {
        printf("header->jumpTable[%02d] = 0x%08lx\n", i, jumpTable[i].offset);
    }
#endif

    return 0;
}

int DecodeBibleBookBriefInfo(
        struct BookBriefInfo* bookBriefInfo,
        int nrOfBooks,
        FILE* fp)
{
    int i = 0;
    
    for (i = 0;i < nrOfBooks;i++)
    {
        bookBriefInfo[i].bookNum    = fread_u16(fp);
        bookBriefInfo[i].bookIndex  = fread_u16(fp);
        bookBriefInfo[i].totalBookRec   = fread_u16(fp);

        fread_blob(bookBriefInfo[i].simpleName, 8, fp);
        fread_blob(bookBriefInfo[i].complexName, 32, fp);
    }

#ifdef PDB_DEBUG
    for (i = 0;i < nrOfBooks;i++)
    {
        printf("version.bookBriefInfo[%d].bookNum       : %d\n",
                i, bookBriefInfo[i].bookNum);
        printf("version.bookBriefInfo[%d].bookIndex     : %d\n",
                i, bookBriefInfo[i].bookIndex);
        printf("version.bookBriefInfo[%d].totalBookRec  : %d\n",
                i, bookBriefInfo[i].totalBookRec);

        printf("version.bookBriefInfo[%d].simpleName    : %s\n",
                i, bookBriefInfo[i].simpleName);
        printf("version.bookBriefInfo[%d].complexName   : %s\n",
                i, bookBriefInfo[i].complexName);
        printf("\n");
    }
#endif

    return 0;
}

int DecodeBibleVersion(
        struct RecordVersion*    version,
        FILE* fp)
{
    fread_blob(version->versionName, 16, fp);
    fread_blob(version->versionInfo, 128, fp);
    version->sepChar = fread_u8(fp);
    version->versionAttrib = fread_u8(fp);

    version->wordIndex      = fread_u16(fp);
    version->totalWordRec   = fread_u16(fp);
    version->totalBooks     = fread_u16(fp);

#ifdef PDB_DEBUG
    printf("version.versionName : %s\n", version->versionName);
    printf("version.versionInfo : ");
        printf_string(version->versionInfo, 128);
        printf("\n");
    printf("version.sepChar     : 0x%02x\n", version->sepChar);

    printf("version.versionAttrib : 0x%02x\n", version->versionAttrib);
    printf("version.wordIndex   : 0x%04x\n", version->wordIndex);
    printf("version.totalWordRec: %d\n", version->totalWordRec);
    printf("version.totalBooks  : %d\n", version->totalBooks);

    printf("\n");
#endif

    DecodeBibleBookBriefInfo(version->bookBriefInfo, version->totalBooks, fp);

    return 0;
}

int DecodeBibleWordTable(
        struct RecordWordTable*  wordTable,
        FILE* fp)
{
    int i = 0;
    wordTable->totalIndexes = fread_u16(fp);

#ifdef PDB_DEBUG
    printf("wordTable.totalIndexes  : %d\n", wordTable->totalIndexes);
#endif
    
    for (i = 0;i < wordTable->totalIndexes;i++)
    {
        
        wordTable->wordBriefData[i].wordLength = fread_u16(fp);
        wordTable->wordBriefData[i].totalWord  = fread_u16(fp);
        wordTable->wordBriefData[i].boolCompressed = fread_u8(fp);
        wordTable->wordBriefData[i].boolNothing = fread_u8(fp);
    }

#ifdef PDB_DEBUG
    for (i = 0;i < wordTable->totalIndexes;i++)
    {
        printf("wordTable.wordBriefData[%d].wordLength      : %d\n",
                i, wordTable->wordBriefData[i].wordLength);
        printf("wordTable.wordBriefData[%d].totalWord       : %d\n",
                i, wordTable->wordBriefData[i].totalWord);
        printf("wordTable.wordBriefData[%d].boolCompressed  : %d\n",
                i, wordTable->wordBriefData[i].boolCompressed);
        printf("wordTable.wordBriefData[%d].boolNothing     : %d\n",
                i, wordTable->wordBriefData[i].boolNothing);
        printf("\n");
    }
#endif

    return 0;
}

int DecodeBibleWordDict(
        struct RecordWordData* wordData,
        const struct RecordWordTable* wordTable,
        FILE* fp)
{
    wordData->offset = ftell(fp);
#ifdef PDB_DEBUG
    printf("wordData : building......\n");
#endif

    return 0;
}

int DecodeBibleBookDetailInfo(
        struct BookDetailInfo* bookDetailInfo,
        FILE* fp)
{
    int i = 0;
    uint16_t numChapters     = 0;
    uint32_t lastVerseNumber = 0;

    //Read number of chapters
    bookDetailInfo->numChapters = fread_u16(fp);
#ifdef PDB_DEBUG
    printf("bookDetailInfo.numChapters : %d\n", bookDetailInfo->numChapters);
#endif

    //Read last verse number table
    numChapters = bookDetailInfo->numChapters;

    bookDetailInfo->lastVerseNumber = 
        (uint16_t*)malloc(sizeof(uint16_t) * numChapters);
    if (bookDetailInfo->lastVerseNumber == NULL)
    {
        MSG_MALLOC_FAIL(bookDetailInfo->lastVerseNumber);
        return -1;
    }
    else
    {
        //fread_blob(bookDetailInfo->lastVerseNumber,
        //        sizeof(uint16_t) * numChapters,
        //        fp);
        for (i = 0;i < numChapters;i++)
        {
            bookDetailInfo->lastVerseNumber[i] = fread_u16(fp);
        }
#ifdef PDB_DEBUG
        printf("bookDetailInfo.lastVerseNumber : ...\n");
#endif
    }

    //Read chapter offset table
    bookDetailInfo->chapterOffset =
        (uint32_t*)malloc(sizeof(uint32_t) * numChapters);
    if (bookDetailInfo->chapterOffset == NULL)
    {
        free(bookDetailInfo->lastVerseNumber);
        bookDetailInfo->lastVerseNumber = NULL;

        MSG_MALLOC_FAIL(bookDetailInfo->chapterOffset);
        return -1;
    }
    else
    {
        //fread_blob(bookDetailInfo->chapterOffset,
        //        sizeof(uint32_t) * numChapters,
        //        fp);
        for (i = 0;i < numChapters;i++)
        {
            bookDetailInfo->chapterOffset[i] = fread_u32(fp);
        }
#ifdef PDB_DEBUG
        printf("bookDetailInfo.chapterOffset : ...\n");
#endif
    }

    //Read verse offset table
    lastVerseNumber = bookDetailInfo->lastVerseNumber[numChapters - 1];
    bookDetailInfo->verseOffset = 
        (uint16_t*)malloc(sizeof(uint16_t) * lastVerseNumber);

    if (bookDetailInfo->verseOffset == NULL)
    {
        free(bookDetailInfo->lastVerseNumber);
        bookDetailInfo->lastVerseNumber = NULL;

        free(bookDetailInfo->chapterOffset);
        bookDetailInfo->chapterOffset = NULL;

        MSG_MALLOC_FAIL(bookDetailInfo->verseOffset);
        return -1;
    }
    else
    {
        //fread_blob(bookDetailInfo->verseOffset,
        //        sizeof(uint16_t) * lastVerseNumber,
        //        fp);
        for (i = 0;i < lastVerseNumber;i++)
        {
            bookDetailInfo->verseOffset[i] = fread_u16(fp);
        }
#ifdef PDB_DEBUG
        printf("bookDetailInfo.verseOffset : ...\n");
#endif
    }

    return 0;
}

int DecodeBibleBookDetailData(
        struct BookDetailData* bookDetailData,
        const struct BookDetailInfo* bookDetailInfo,
        FILE* fp)
{
    bookDetailData->offset = ftell(fp);
    bookDetailData->nrOfWords = 0;
    bookDetailData->verseBuffer = NULL;
#ifdef PDB_DEBUG
    printf("bookDetailData : building......\n");
#endif
    return 0;
}

int DecodeBibleBookDetail(
        struct RecordBookDetail* bookDetail,
        const struct JumpTable*  jumpTable,
        const struct BookBriefInfo* bookBriefInfo,
        FILE* fp)
{
    DecodeBibleBookDetailInfo(&(bookDetail->bookDetailInfo), fp);
#ifdef PDB_DEBUG
    printf("\n");
#endif

    JumpToOffset(jumpTable[bookBriefInfo->bookIndex + 1].offset, fp);
    DecodeBibleBookDetailData(
            &(bookDetail->bookDetailData), 
            &(bookDetail->bookDetailInfo),
            fp);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
//Global PDB interface define
void JumpToOffset(uint32_t offset, FILE* fp)
{
#ifdef PDB_DEBUG
    printf(" * Jump to offset 0x%04lx\n",offset);
#endif
    fseek(fp,offset,0);
}

struct PDBLayout* DecodePDBFile(FILE* fp)
{
    struct PDBLayout* pdbLayout = NULL;
    int i = 0;
    uint32_t offset = 0u;
    uint16_t bookIndex = 0u;

    pdbLayout = (struct PDBLayout*)malloc(sizeof(struct PDBLayout));
    if (pdbLayout == NULL)
    {
        MSG_MALLOC_FAIL(pdbLayout);
        return NULL;
    }
    else
    {
        memset(pdbLayout, 0, sizeof(struct PDBLayout));
    }
    
    JumpToOffset(0, fp);

    DecodePDBHeader(&(pdbLayout->header), fp);
    DecodePDBJumpTable(pdbLayout->header.jumpTable, 
            pdbLayout->header.numRecs, 
            fp);
#ifdef PDB_DEBUG
    printf("\n");
#endif

    JumpToOffset(pdbLayout->header.jumpTable[0].offset, fp);
    DecodeBibleVersion(&(pdbLayout->version), fp);
#ifdef PDB_DEBUG
    printf("\n");
#endif

    JumpToOffset(pdbLayout->header.jumpTable[1].offset, fp);
    DecodeBibleWordTable(&(pdbLayout->wordTable), fp);
#ifdef PDB_DEBUG
    printf("\n");
#endif

    JumpToOffset(pdbLayout->header.jumpTable[2].offset, fp);
    DecodeBibleWordDict(
            &(pdbLayout->wordData),
            &(pdbLayout->wordTable),
            fp);
#ifdef PDB_DEBUG
    printf("\n");
#endif

    //Decode Book
    for (i = 0;i < pdbLayout->version.totalBooks;i++)
    {
        bookIndex = pdbLayout->version.bookBriefInfo[i].bookIndex;
        offset    = pdbLayout->header.jumpTable[bookIndex].offset;

        JumpToOffset(offset, fp);
        DecodeBibleBookDetail(
                pdbLayout->bookDetail + i,
                pdbLayout->header.jumpTable,
                &(pdbLayout->version.bookBriefInfo[i]),
                fp);
#ifdef PDB_DEBUG
        printf("\n");
#endif
    };

    return pdbLayout;
}

void FreePDBFile(struct PDBLayout* pdbLayout)
{
    int i = 0;

    for (i = 0;i < pdbLayout->version.totalBooks;i++)
    {
        if (pdbLayout->bookDetail[i].bookDetailInfo.lastVerseNumber)
        {
            free(pdbLayout->bookDetail[i].bookDetailInfo.lastVerseNumber);
            pdbLayout->bookDetail[i].bookDetailInfo.lastVerseNumber = NULL;
        }
        if (pdbLayout->bookDetail[i].bookDetailInfo.chapterOffset)
        {
            free(pdbLayout->bookDetail[i].bookDetailInfo.chapterOffset);
            pdbLayout->bookDetail[i].bookDetailInfo.chapterOffset = NULL;
        }
        if (pdbLayout->bookDetail[i].bookDetailInfo.verseOffset)
        {
            free(pdbLayout->bookDetail[i].bookDetailInfo.verseOffset);
            pdbLayout->bookDetail[i].bookDetailInfo.verseOffset = NULL;
        }
    }

    free(pdbLayout);
    pdbLayout = NULL;
}

