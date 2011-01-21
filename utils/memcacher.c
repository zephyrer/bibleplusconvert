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
#include "memcacher.h"

struct MEMORYCACHER
{
    MCBLOB* pBLOB;
    int     maxNumberOfItem;
};

MEMORYCACHER* memcacher_init(int maxNumberOfItem)
{
    MEMORYCACHER* pMemoryCacher = NULL;
    pMemoryCacher = (MEMORYCACHER*)malloc(sizeof(MEMORYCACHER));

    if (pMemoryCacher == NULL)
    {
        return NULL;
    }
    else
    {
        pMemoryCacher->maxNumberOfItem = maxNumberOfItem;
        pMemoryCacher->pBLOB = (MCBLOB*)malloc(sizeof(MCBLOB) * maxNumberOfItem);

        if (pMemoryCacher->pBLOB == NULL)
        {
            free(pMemoryCacher);
            pMemoryCacher = NULL;
            return NULL;
        }
        else
        {
            memset(pMemoryCacher->pBLOB, 0, sizeof(MCBLOB) * maxNumberOfItem);
            return pMemoryCacher;
        }
    }
}

MC memcacher_fini(MEMORYCACHER* pMemoryCacher)
{
    int i = 0;
    int maxNumberOfItem = 0;
    MCBLOB* pBLOB = NULL;

    if (pMemoryCacher == NULL)
    {
        return MC_FAIL;
    }
    else
    {
        maxNumberOfItem = pMemoryCacher->maxNumberOfItem;
        pBLOB = pMemoryCacher->pBLOB;
        for (i = 0;i < maxNumberOfItem;i++, pBLOB++)
        {
            if (pBLOB->pBuffer != NULL)
            {
                free(pBLOB->pBuffer);
                pBLOB->pBuffer = NULL;
                pBLOB->nrOfBytes = 0;
            }
        }

        free(pMemoryCacher->pBLOB);
        pMemoryCacher->pBLOB = NULL;

        free(pMemoryCacher);
        pMemoryCacher = NULL;

        return MC_OK;
    }
}

MC memcacher_push(MEMORYCACHER* pMemoryCacher, long bookIndex, MCBLOB* pBLOB)
{
    MCBLOB* pBLOBItme = NULL;
    if (pMemoryCacher == NULL)
    {
        return MC_FAIL;
    }
    else
    {
        if ((0 <= bookIndex) && (bookIndex < pMemoryCacher->maxNumberOfItem))
        {
            pBLOBItme = pMemoryCacher->pBLOB + bookIndex;

            if (pBLOBItme->pBuffer != NULL)
            {
                free(pBLOBItme->pBuffer);
                pBLOBItme->pBuffer = NULL;
            }

            pBLOBItme->nrOfBytes = pBLOB->nrOfBytes;
            pBLOBItme->pBuffer = pBLOB->pBuffer;
            
            pBLOB->nrOfBytes = 0;
            pBLOB->pBuffer = NULL;

            return MC_OK;
        }
        else
        {
            return MC_FAIL;
        }
    }
}

MC memcacher_pull(MEMORYCACHER* pMemoryCacher, long bookIndex, const MCBLOB** pBLOB)
{
    if (pMemoryCacher == NULL)
    {
        return MC_FAIL;
    }
    else
    {
        if ((0 <= bookIndex) && (bookIndex < pMemoryCacher->maxNumberOfItem))
        {
            if (pMemoryCacher->pBLOB[bookIndex].pBuffer != NULL)
            {
                *pBLOB = pMemoryCacher->pBLOB + bookIndex;
                return MC_OK;
            }
            else
            {
                return MC_FAIL;
            }
        }
        else
        {
            return MC_FAIL;
        }
    }
}

MC memcacher_erase(MEMORYCACHER* pMemoryCacher, long bookIndex)
{
    MCBLOB* pBLOBItme = NULL;
    if (pMemoryCacher == NULL)
    {
        return MC_FAIL;
    }
    else
    {
        if ((0 <= bookIndex) && (bookIndex < pMemoryCacher->maxNumberOfItem))
        {
            pBLOBItme = pMemoryCacher->pBLOB + bookIndex;
            if (pBLOBItme->pBuffer != NULL)
            {
                free(pBLOBItme->pBuffer);
                pBLOBItme->pBuffer = NULL;
                pBLOBItme->nrOfBytes = 0;
                return MC_OK;
            }
            else
            {
                return MC_FAIL;
            }
        }
        else
        {
            return MC_FAIL;
        }
    }
}


