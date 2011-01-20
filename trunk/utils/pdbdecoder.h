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
//The max number of record is 1024, and this pdb
//file will be max at 1024 * 4096 == 4Mb.
//It is enough for bible+ pdb.
#define JUMPTABLE_MAX   1024

//I define this max length because not
//any word in bible max than 64.
#define WORDTABLE_MAX   64

//Number of Book in bible is 66
//It is magic digit:)
#define BOOKNUM_MAX     66

///////////////////////////////////////////////////////////////////////////////
//Global PDB struct define
//Jump table, It record the offset of each record!
//the value from the beginning of file.
//So it is easy to jump to every record.
struct JumpTable
{
    uint32_t    offset;
    uint8_t     reserved[4];
};

//Brief information of one book
//record book start index(of jump table.)
//totalIndexes is total number of data record.
//each book contain at lease two record.
//one is book chapter/verse record
//another is text data record.
struct BookBriefInfo
{
    uint16_t    bookNum;
    uint16_t    bookIndex;
    uint16_t    totalBookRec;
    uint8_t     simpleName[8];
    uint8_t     complexName[32];
};

//Word dict brief information
//each word dict hold a set of wordLength's word.
//if boolCompressed is 1, it means that 
//the data in this dict is compress, data is
//index too, you need to find in other dict:)
struct WordBriefData
{
    uint16_t    wordLength;
    uint16_t    totalWord;
    uint8_t     boolCompressed;
    uint8_t     boolNothing;
};

//the first struct in all book record.
//it contain data need for locate the chapter and verse
struct BookDetailInfo
{
    uint16_t    numChapters;
    uint16_t*   lastVerseNumber;//Size is numChapters
    uint32_t*   chapterOffset;//Size is numChapters
    uint16_t*   verseOffset;//Size is lastVerseNumber[numChapters]
};

//book data begin offset 
//It does not in pdb, I add this for easy jump:)
struct BookDetailData
{
    uint32_t    offset;
    uint32_t    nrOfWords;
    uint16_t*   verseBuffer;
};


///////////////////////////////////////////////////////////
//This is pdb file header, every pdb file have it.
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

//Record of book version
//record bible version name(kjv,niv,etc...)
//and other important things.
//
//be sure of sepChar, if this is none zero
//it will be add to separate word.
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

//Record of dict(word table).
struct RecordWordTable
{
    uint16_t    totalIndexes;
    struct WordBriefData     wordBriefData[WORDTABLE_MAX];
};

//Record of dict data.
//It is not in pdb, I add it to easy jump.
struct RecordWordData
{
    uint32_t    offset;
};

//Record of book detail info.
struct RecordBookDetail
{
    struct BookDetailInfo   bookDetailInfo;
    struct BookDetailData   bookDetailData;
};

///////////////////////////////////////////////////////////
//The pdb format layout, easy to read:)
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
/**
 * @brief set file to offset. such like jump.
 */
void JumpToOffset(uint32_t offset, FILE* fp);

/**
 * @brief Decode pdb file from FILE.
 *
 * @return the struct than contain pdb decoded data.
 * @tips the data of word, data of text not decoded! you need to 
 *      decode it by your self. So don't close file, if you want
 *      to decode text.
 */
struct PDBLayout* DecodePDBFile(FILE* fp);

/**
 * @brief free pdb struct
 * @details
 *      DecodePDBFile create many malloc data, need this interface
 *      to free them.
 *
 *      Don't forget!!!
 */
void FreePDBFile(struct PDBLayout* pdbLayout);

#endif

