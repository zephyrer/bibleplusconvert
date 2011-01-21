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
    MEMORYCACHER* pMemoryCacher = NULL;
    MCBLOB blob;
    const MCBLOB* pBLOB = NULL;
    char buffer[] = "Hello world!";

    blob.pBuffer = strdup(buffer);
    blob.nrOfBytes = sizeof(buffer);

    pMemoryCacher = memcacher_init(10);

    if (memcacher_push(pMemoryCacher, 1, &blob) == MC_OK)
    {
        blob.pBuffer = NULL;
        blob.nrOfBytes = 0;
        printf("Success push in!\n");
        if (memcacher_pull(pMemoryCacher, 1, &pBLOB) == MC_OK)
        {
            printf("Success pull : %s\n", pBLOB->pBuffer);
        }
    }

    memcacher_fini(pMemoryCacher);
    pMemoryCacher = NULL;

    return 0;
}

