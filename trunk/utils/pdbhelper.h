#ifndef PDBHELPER_H
#define PDBHELPER_H

#include "pdbcommon.h"
#include "pdbdecoder.h"
#include "dictUtils.h"

///////////////////////////////////////////////////////////////////////////////
//Global Vserse return value
#define MAX_WORD_LENGTH 128

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
 * @tip bookToView start with 1
 */
uint16_t GetNumberOfChapters(
        const struct PDBLayout* pdbLayout,
        uint8_t  bookToView);

/**
 * @brief Get number of verse from pdb
 * @tip bookToView, chapterToView both start with 1
 */
uint16_t GetNumberOfVerses(
        const struct PDBLayout* pdbLayout,
        uint8_t  bookToView,
        uint8_t  chapterToView);
/**
 * @brief Get verse offset from pdb
 * @tip bookToView, chapterToView, verseToView all start with 1
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
 * @param[in]   pdbLayout pdb layout file
 * @param[in]   wordIndex start with 1.
 *
 * @tip wordIndex from 1
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
 * @tip bookToView, chapterToView, verseToView all start with 1
 */
struct VerseData DecompressBookAllVerseData(
        const struct PDBLayout* pdbLayout,
        uint8_t  bookToView,
        FILE* fp);

/**
 * @brief decode verse , map word index to real word.
 *
 * @tip buffer must big enough! may be 4096.
 */
void DecompressVerse(
        const struct PDBLayout* pdbLayout,
        const uint16_t* verseData, 
        const uint32_t  verseNROfWords,
        uint32_t verseOffset,
        uint16_t nrOfWords,
        const struct WordBook* pWordBook,
        FILE* fp,
        uint8_t* buffer);

/**
 * @brief build word dict to ram, for speed up random access.
 */
struct WordBook* BuildWordDictInRam(
        const struct PDBLayout* pdbLayout,
        FILE* fp);

#endif

