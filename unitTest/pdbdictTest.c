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
#include "pdbcommon.h"
#include "pdbdecoder.h"
#include "pdbhelper.h"
#include "byteUtils.h"
#include "memcat.h"

//#define TEST_DEBUG

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
    FILE* fp;
    struct PDBLayout* pdbLayout = NULL;

    if (argc == 2)
    {
        fp = fopen(argv[1], "rb");
        if (fp == NULL)
        {
            perror("open pdb file fail");
            return -1;
        }
    }
    else
    {
        printf("need input file\n");
        return -2;
    }

    pdbLayout = DecodePDBFile(fp);

    if (pdbLayout != NULL)
    {
        PDBPrintWordDict(pdbLayout, fp);

        //释放PDBLayout
        FreePDBFile(pdbLayout);
        pdbLayout = NULL;
    }

    //关闭输入文件
    fclose(fp);
    fp = NULL;

    return 0;
}

