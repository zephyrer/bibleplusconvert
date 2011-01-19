#include <stdio.h>
#include <stdlib.h>

#include "dictUtils.h"

int main(int argc, char* argv[])
{
    const char* p = NULL;
    const int  nrOfWords = 10;
    const char* dict[5] = {"John","Jack","David","Johah","Ken"};
    int i = 0;

    struct WordBook* wordBook = NULL;

    wordBook = WordBook_init(nrOfWords);
    CHECK_RETURN_IF_ARGUMENT_IS_NULL(wordBook);

    for (i = 0;i < 5;i++)
    {
        if (!WordBook_AppendWord(wordBook, dict[i]))
        {
            OPERATION_FAIL_MSG("Append word");
            break;
        }
    }

    for (i = 0;i < 5;i++)
    {
        if (WordBook_GetWord(wordBook,i,&p))
        {
            printf("%d : %s\n", i, p);
        }
        else
        {
            OPERATION_FAIL_MSG("Get word");
            break;
        }
    }

    WordBook_fini(wordBook);
    wordBook = NULL;

    return 0;
}
