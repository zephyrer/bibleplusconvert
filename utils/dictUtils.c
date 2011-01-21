#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dictUtils.h"

///////////////////////////////////////////////////////////////////////////////
//Local struct declare
typedef char* WordItem;

struct WordBook
{
    int         nrOfUseWords;
    int         nrOfMaxWords;
    WordItem*   wordList;
};


///////////////////////////////////////////////////////////////////////////////
//Global interface define
struct WordBook* WordBook_init(int nrOfMaxWords)
{
    struct WordBook* ptmp = NULL;

    ptmp = (struct WordBook*)malloc(sizeof(struct WordBook));
    if (ptmp != NULL)
    {
        ptmp->nrOfUseWords = 0;
        ptmp->nrOfMaxWords = nrOfMaxWords;

        ptmp->wordList  = (WordItem*)malloc(sizeof(WordItem) * nrOfMaxWords);
        if (ptmp->wordList != NULL)
        {
            memset(ptmp->wordList, 0, sizeof(WordItem) * nrOfMaxWords);
        }
        else
        {
            MALLOC_FAIL_MSG("sizeof(WordItem) * nrOfMaxWords");
        }
    }
    else
    {
        MALLOC_FAIL_MSG("struct WordBook");
    }

    return ptmp;
}

WBK_RESULT WordBook_fini(struct WordBook* pWordBook)
{
    int i = 0;
    int nrOfWords = 0;
    WordItem* wordList = NULL;
    CHECK_RETURN_IF_ARGUMENT_IS_NULL(pWordBook);

    nrOfWords = pWordBook->nrOfUseWords;
    wordList  = pWordBook->wordList;

    if (wordList != NULL)
    {
        for (i = 0;i < nrOfWords;i++)
        {
            if (wordList[i] != NULL)
            {
                free(wordList[i]);
                wordList[i] = NULL;
            }
        }

        free(pWordBook->wordList);
        pWordBook->wordList = NULL;
    }
    else
    {
        printf("Error : pWordBook->wordList is NULL\n");
        return WBK_FAIL;
    }

    free(pWordBook);
    pWordBook = NULL;

    return WBK_OK;
}

WBK_RESULT WordBook_AppendWord(struct WordBook* pWordBook, const char* pWord)
{
    WordItem wordItem = NULL;

    CHECK_RETURN_IF_ARGUMENT_IS_NULL(pWordBook);
    CHECK_RETURN_IF_ARGUMENT_IS_NULL(pWord);

    wordItem = strdup(pWord);

    if (wordItem != NULL)
    {
        if (pWordBook->nrOfUseWords < pWordBook->nrOfMaxWords)
        {
            pWordBook->wordList[pWordBook->nrOfUseWords] = wordItem;
            pWordBook->nrOfUseWords++;

            return WBK_OK;
        }
        else
        {
            OPERATION_FAIL_MSG("Book Full!");
            free(wordItem);
            wordItem = NULL;

            return WBK_FAIL;
        }
    }
    else
    {
        MALLOC_FAIL_MSG("strdup");
        return WBK_FAIL;
    }
}

WBK_RESULT WordBook_AppendBLOB(struct WordBook* pWordBook, const char* pWord, int nrOfBytes)
{
    WordItem wordItem = NULL;

    CHECK_RETURN_IF_ARGUMENT_IS_NULL(pWordBook);
    CHECK_RETURN_IF_ARGUMENT_IS_NULL(pWord);

    wordItem = (WordItem)malloc(nrOfBytes);
    memcpy(wordItem, pWord, nrOfBytes);

    if (wordItem != NULL)
    {
        if (pWordBook->nrOfUseWords < pWordBook->nrOfMaxWords)
        {
            pWordBook->wordList[pWordBook->nrOfUseWords] = wordItem;
            pWordBook->nrOfUseWords++;

            return WBK_OK;
        }
        else
        {
            OPERATION_FAIL_MSG("Book Full!");
            free(wordItem);
            wordItem = NULL;

            return WBK_FAIL;
        }
    }
    else
    {
        MALLOC_FAIL_MSG("strdup");
        return WBK_FAIL;
    }
}

WBK_RESULT WordBook_GetWord(
        const struct WordBook* pWordBook, 
        int index,
        const char** pWord
        )
{
    CHECK_RETURN_IF_ARGUMENT_IS_NULL(pWordBook);
    CHECK_RETURN_IF_ARGUMENT_IS_NULL(pWord);

    if (index < pWordBook->nrOfUseWords)
    {
        *pWord = pWordBook->wordList[index];
        return WBK_OK;
    }
    else
    {
        OPERATION_FAIL_MSG("index overflow");
        return WBK_FAIL;
    }
}


