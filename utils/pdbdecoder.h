#ifndef PDBDECODER_H
#define PDBDECODER_H


/**
 * @file
 * @tips
 *  All bytes in pdb is big endian!
 *
 * @attention
 *  This code compile using gcc(mingw).
 *
 * @binary layout
 *  struct Header
 *  struct JumpTable
 *  ...record...
 *      record0:
 *          struct Version
 *          struct BookMetaInfo[]
 *      record1:
 *          struct WordTableNumber
 *          struct WordTable[]
 *      record2-N:(N is first book)
 *          string wordDict
 *
 *      recordN:
 *          struct BookInfoNum
 *          struct BookInfoLastVerseNumber[]
 *          struct BookInfoChapterOffset[]
 *          struct BookInfoLastVerseNumber[]
 *
 *      recordN+1:
 *          struct BookInfoNum
 *          struct BookInfoLastVerseNumber[]
 *          struct BookInfoChapterOffset[]
 *          struct BookInfoLastVerseNumber[]
 *
 *      ......
 *  ...record...
 *      
 */

#include "pdbcommon.h"

///////////////////////////////////////////////////////////////////////////////
//Global macro define
//最大的Record个数就是Jump Table的最大个数
//目前先定为1024，一个Record的最大大小是2Kb
//所以这个文件的最大大小为2Mb
#define JUMPTABLE_MAX   1024

//字典的个数定为64，因为很少有超过64个字母
//的单词
#define WORDTABLE_MAX   64

//圣经的最大个数只能是66，不用想了
#define BOOKNUM_MAX     66

///////////////////////////////////////////////////////////////////////////////
//Global PDB struct define

//跳转表，相当于整个文件中各个块的整体索
//引信息，很重要
struct JumpTable
{
    uint32_t    offset;
    uint8_t     reserved[4];
};

//书卷的摘要信息
struct BookBriefInfo
{
    uint16_t    bookNum;
    uint16_t    bookIndex;
    uint16_t    totalBookRec;
    uint8_t     simpleName[8];
    uint8_t     complexName[32];
};

//单词本的摘要信息
struct WordBriefData
{
    uint16_t    wordLength;
    uint16_t    totalWord;
    uint8_t     boolCompressed;
    uint8_t     boolNothing;
};

//书卷的详细信息
struct BookDetailInfo
{
    uint16_t    numChapters;
    uint16_t*   lastVerseNumber;//Size is numChapters
    uint32_t*   chapterOffset;//Size is numChapters
    uint16_t*   verseOffset;//Size is lastVerseNumber[numChapters]
};

//书卷的文字
struct BookDetailData
{
    uint32_t    offset;
    uint32_t    nrOfWords;
    uint16_t*   verseBuffer;
};


///////////////////////////////////////////////////////////
//以下为PDB的文件格式定义
//这是PDB文件头，每个PDB文件都有这个东西
struct PDBHeader
{
    uint8_t     fileName[32];
    uint16_t    fileAttributes;
    uint16_t    versionNumber;
    uint32_t    createDate;
    uint32_t    modificationDate;
    uint32_t    lastBackupDate;

    uint32_t    modificationNumber;
    uint32_t    appInfoArea;
    uint32_t    sortInfoArea;
    uint8_t     dataBaseType[4];
    uint8_t     creatorID[4];
    uint8_t     uniqueIDSeed[4];
    uint32_t    nextRecord;
    uint16_t    numRecs;

    struct JumpTable    jumpTable[JUMPTABLE_MAX];
};

//这本Bible的简要信息（版本信息）
struct RecordVersion
{
    uint8_t     versionName[16];
    uint8_t     versionInfo[128];
    uint8_t     sepChar;
    uint8_t     versionAttrib;
    uint16_t    wordIndex;
    uint16_t    totalWordRec;
    uint16_t    totalBooks;
    struct BookBriefInfo     bookBriefInfo[BOOKNUM_MAX];
};

//字典的简要信息
struct RecordWordTable
{
    uint16_t    totalIndexes;
    struct WordBriefData     wordBriefData[WORDTABLE_MAX];
};

//字典的数据开始偏移
struct RecordWordData
{
    uint32_t    offset;
};

//书卷的详细信息
struct RecordBookDetail
{
    struct BookDetailInfo   bookDetailInfo;
    struct BookDetailData   bookDetailData;
};

///////////////////////////////////////////////////////////
//PDB 文件格式
struct PDBLayout
{
    //PDB data
    struct PDBHeader        header;

    //Record books info
    struct RecordVersion    version;

    //Record word info
    struct RecordWordTable  wordTable;

    //Record word data
    struct RecordWordData   wordData;

    //Record book data
    struct RecordBookDetail bookDetail[BOOKNUM_MAX];
};


///////////////////////////////////////////////////////////////////////////////
//Global PDB interface declare
void JumpToOffset(uint32_t offset, FILE* fp);
struct PDBLayout* DecodePDBFile(FILE* fp);
void FreePDBFile(struct PDBLayout* pdbLayout);

#endif

