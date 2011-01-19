#ifndef PDBHELPER_H
#define PDBHELPER_H

#include "pdbcommon.h"
#include "pdbdecoder.h"

///////////////////////////////////////////////////////////////////////////////
//Global Vserse return value
#define VERSEOFFSET_INVAILD 0xFFFFFFFF
struct VerseOffset
{
    uint32_t offset;
    uint16_t nrOfWords;
};

struct WordOffset
{
    uint32_t offset;
    uint16_t nrOfBytes;
    uint8_t  boolCompressed;
};

struct VerseData
{
    uint16_t* verseBuffer;
    uint32_t  nrOfWords;
};

/**
 * @brief Get number of chapter from pdb
 * @tip bookToView是从1开始计数
 */
uint16_t GetNumberOfChapters(
        const struct PDBLayout* pdbLayout,
        uint8_t  bookToView);

/**
 * @brief Get number of verse from pdb
 * @tip bookToView, chapterToView都是从1开始计数
 */
uint16_t GetNumberOfVerses(
        const struct PDBLayout* pdbLayout,
        uint8_t  bookToView,
        uint8_t  chapterToView);
/**
 * @brief Get verse offset from pdb
 * @tip bookToView, chapterToView, verseToView 都是从1开始计数
 */
struct VerseOffset GetVerseOffsetFromPDB(
        const struct PDBLayout* pdbLayout,
        uint8_t  bookToView,
        uint8_t  chapterToView,
        uint16_t verseToView);

/**
 * @brief Get word offset from pdb
 * @details     
 *
 * @param[in]   pdbLayout PDB布局文件
 * @param[in]   wordIndex 需要查找的单词在整个字典中的索引号
 *
 * @tip wordIndex 从1开始计数
 */
struct WordOffset GetWordOffsetFromPDB(
        const struct PDBLayout* pdbLayout,
        uint16_t wordIndex);

/**
 * @brief Print one given book info
 */
uint16_t PDBPrintOneBooKInfo(
        const struct PDBLayout* pdbLayout,
        uint16_t bookToView);

/**
 * @brief Print all book info
 */
uint16_t PDBPrintAllBookInfo(
        const struct PDBLayout* pdbLayout);

/**
 * @brief Print word dict
 */
uint16_t PDBPrintWordDict(
        const struct PDBLayout* pdbLayout,
        FILE* fp);

/**
 * @brief Get verse data form pdb
 *
 * @tip bookToView, chapterToView, verseToView 都是从1开始计数
 */
struct VerseData DecompressBookAllVerseData(
        const struct PDBLayout* pdbLayout,
        uint8_t  bookToView,//From 1
        FILE* fp);

#endif

