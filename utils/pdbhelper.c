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
#include "byteUtils.h"
#include "pdbhelper.h"

///////////////////////////////////////////////////////////////////////////////
//Local interface declare
///解码14bit/16bit压缩的数据
static uint16_t DecompressVerseData(
        const uint16_t* compressedVerseData,
        uint32_t nrOfDataWords,
        uint16_t* decompressedVerseData,
        uint32_t nrOfDecompressedDataBytes
        );


///////////////////////////////////////////////////////////////////////////////
//Global interface define
/**
 * ------------------------------------------------------------------
 * verseOffset : 指向下一句距离本章开始的偏移，本章最后一节对应的
 *          verseOffset指向下一章的第一句
 * lastVerseNumber : 记录本章的最后一个verse距离的标号，把所有的verse
 *          序号累加了，所以这个值的意义是本章最后一句是本书的第几句。
 *
 * 以下是Gen的信息
 * ------------------------------------------------------------------
 * chapter  verse   verseOffset lastVerseNumber chapterOffset
 *    1        1       [0]=22                       [0]=0
 *             2       [1]=48                    
 *                     ...         
 *             29      [28]=722
 *             30      [29]=760
 *             31      [30]=784     [0]=31
 *  -----------------
 *    2        1       [31]=17                      [1]=784
 *                     ...
 *                     [54]=597
 *             25      [55]=612     [1]=56
 *  -----------------
 *    3        1       [56]=36                      [2]=1396
 *             2       [57]=53
 *
 * byteOffset(chapter 2, 25) 
 * = chapterOffset[2 - 1] + verseOffset[lastVerseNumber[(2 - 1) - 1] + 25 -2]
 * = 784 + verseOffset[31 + 25 -2]
 * = 784 + verseOffset[54]
 * = 784 + 597
 * = 1381
 */
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
        uint8_t  bookToView,//From 1
        uint8_t  chapterToView,//From 1
        uint16_t verseToView)//From 1
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

    //从章节号得到章节的第一个Verse的偏移
    nchaptersFirstVerse = 0; 

    if (chapterToView > 1)
    {
        //lastVerseNumber[i] 保存了第(i = chapterToView - 1)章的最后一节
        //等价于下一章的开始
        nchaptersFirstVerse = lastVerseNumber[chapterToView - 2];
    }

    nverseOffset = chapterOffset[chapterToView - 1];

    //获得这节的偏移
    if (verseToView > 1)
    {
        nverseOffset += 
            verseOffset[nchaptersFirstVerse + verseToView - 2];
    }

    //计算这节的长度
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
    /*
    //加上数据起始地址
    nverseOffset += 
        pdbLayout->bookDetail[bookToView - 1].bookDetailData.offset;
    */
    nrOfWords &= 0xFFF;

    //保存计算结果
    ret_value.offset = nverseOffset;
    ret_value.nrOfWords = nrOfWords;

    return ret_value;
}

/**
 * 字典的算法
 * 
 * 每个wordBriefData都有一个属性：单词个数
 *
 * 需要先判断 index 属于哪个dict，然后累加之前的数据，得到word offset，
 * 再加上word data的起始offset
 *
 */
struct WordOffset GetWordOffsetFromPDB(
        const struct PDBLayout* pdbLayout,
        uint16_t wordIndex)
{
    uint16_t nrOfWordTable = 0u;
    uint32_t wordDictOffset = 0u;
    const struct WordBriefData* wordBriefData = NULL;
    int i = 0;
    ///wordIndex属于的字典
    int indexOfDict = 0;
    ///之前的word的个数的统计
    uint16_t preIndex = 0u;
    ///查询的字在本字典里的序号
    uint16_t currIndex = 0u;
    ///字的数据在文件中的偏移
    uint32_t wordOffset = 0u;

    struct WordOffset ret_value;

    ret_value.offset = 0u;
    ret_value.nrOfBytes = 0u;
    ret_value.boolCompressed = 0u;

    //保存一些数据
    nrOfWordTable = pdbLayout->wordTable.totalIndexes;
    wordBriefData = pdbLayout->wordTable.wordBriefData;
    wordDictOffset = pdbLayout->wordData.offset;

    //判断当前的wordIndex属于哪个字典
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

    //计算本字典之前的字典们占用的总Offset
    for (i = 0;i < indexOfDict;i++)
    {
        wordOffset += wordBriefData[i].totalWord * wordBriefData[i].wordLength;
    }

    //计算本字在本字典之前的数据占用的Offset
    wordOffset += wordBriefData[indexOfDict].wordLength * currIndex;

    //加上字典数据文件起始偏移
    wordOffset += pdbLayout->wordData.offset;

    //保存返回值
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
        uint8_t  bookToView,//From 1
        FILE* fp)
{
    //数据区起始偏移
    uint32_t dataBaseOffset = 0u;
    //未解压前数据的长度（字节个数）
    uint32_t nrOfDataBytes = 0u;
    //未解压前数据的长度（字个数）
    uint32_t nrOfDataWords = 0u;
    //解压后数据的长度（字节个数）
    uint32_t nrOfDecompressedDataBytes = 0u;

    uint32_t fileEndOffset = 0u;
    //本书的第一个数据区的索引号
    uint16_t  bookDataIndex = 0u;
    //下一卷书的Details记录的索引号
    uint16_t  bookNextIndex  = 0u;

    //动态解析的数据
    uint16_t* compressedVerseData = NULL;
    uint16_t* decompressedVerseData = NULL;

    int i = 0;

    struct VerseData ret_value;

    ret_value.verseBuffer = NULL;
    ret_value.nrOfWords   = 0u;

    //记录文件的最大值
    fseek(fp,0,SEEK_END);
    fileEndOffset = ftell(fp);

    //puts(__func__);

    //如果这不是最后一卷书，那么用下一卷书的开始可以得到本卷书的数据大小
    //一般的布局是这样的
    //Record 书的chapter、verse信息
    //Record 书的数据
    //Record 书的数据
    //
    //Record 书的chapter、verse信息
    //Record 书的数据
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
        //如果只有1卷书，那么bookToView 1 == 1，是最后一卷了
        //如果有66眷属，那么只有66 == 66
        bookDataIndex = pdbLayout->version.bookBriefInfo[bookToView - 1].bookIndex + 1;
        nrOfDataBytes = 
            fileEndOffset - 
            pdbLayout->header.jumpTable[bookDataIndex].offset;
    }

    //申请动态数据
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
            //申请解码后的buffer
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

    //打印调试信息
#ifdef PDB_DEBUG
    printf("Book[%d].nrOfDataBytes = %ld\n",
            (bookToView - 1), nrOfDataBytes);
    printf("Book[%d].nrOfDecompressedDataBytes = %ld\n",
            (bookToView - 1), nrOfDecompressedDataBytes);
#endif

    //如果以上正常，那么读取原始数据（同时进行大小端转换）
    //然后直接解码
    if ((nrOfDataBytes > 0) && 
        (compressedVerseData != NULL) && 
        (decompressedVerseData != NULL))
    {
        //跳转到数据区，并读取数据
        fseek(fp, dataBaseOffset, 0);
        nrOfDataWords = nrOfDataBytes / 2;
        for (i = 0;i < nrOfDataWords;i++)
        {
            compressedVerseData[i] = fread_u16(fp);
        }

        //开始解码数据
        DecompressVerseData(compressedVerseData, nrOfDataWords,
                decompressedVerseData, nrOfDecompressedDataBytes);

        free(compressedVerseData);
        compressedVerseData = NULL;

        //保存返回结果
        ret_value.verseBuffer = decompressedVerseData;
        ret_value.nrOfWords   = (nrOfDecompressedDataBytes + 1) / 2;
    }

    return ret_value;
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

    //开始解码数据
    pByteStream = ByteStreamBit_Init();

#ifdef PDB_DEBUG
    printf("compressed   : ");
#endif
    for (i = 0;i < nrOfDataWords;i++)
    {
        //放入编码后的数据
#ifdef PDB_DEBUG
        set_color(C_LIGHT_GREEN);
        printf("%04x ", compressedVerseData[i]);
#endif

        ByteStreamBit_AppendU16(pByteStream,compressedVerseData[i]);

#ifdef PDB_DEBUG
        set_color(C_GREY);
#endif

        //每7个16bit对应8个14bit的数据，
        //所以放入7个数据之后清空一次
        if ((i % 7) == 6)
        {
            //打印调试信息
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
            //printf("endOfDecompressedData1 %d\n", endOfDecompressedData);

            //保存解码后的数据
            for (j = 0;j < 8;j++)
            {
                wordIndex = ByteStreamBit_GetU14(pByteStream,j);
                decompressedVerseData[endOfDecompressedData++] = wordIndex;
                //printf("%04x ", wordIndex);
            }

            //printf("endOfDecompressedData2 %d\n", endOfDecompressedData);

            ByteStreamBit_Zero(pByteStream);
        }
    }

    //如果跳出循环之前还未处理最后一次的，那么在这里处理
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

        //保存解码数据
        for (j = 0;j < ByteStreamBit_GetNrOfU14Bytes(pByteStream);j++)
        {
            wordIndex = ByteStreamBit_GetU14(pByteStream,j);
            decompressedVerseData[endOfDecompressedData++] = wordIndex;
        }
    }

    //释放解码器
    ByteStreamBit_Fini(pByteStream);
    pByteStream = NULL;

    return endOfDecompressedData;
}

