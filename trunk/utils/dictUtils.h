#ifndef DICTUTILS_H
#define DICTUTILS_H

///////////////////////////////////////////////////////////////////////////////
//Global macro define
#define CHECK_RETURN_IF_ARGUMENT_IS_NULL(p) \
    if ((p) == NULL) \
    { \
        printf("Error : pointer %s is NULL at %s:%d\n", #p, __FILE__,__LINE__); \
        return WBK_FAIL; \
    }

#define MALLOC_FAIL_MSG(msg) \
    printf("Error : malloc \"%s\" fail at %s:%d\n",(msg),__FILE__,__LINE__);

#define OPERATION_FAIL_MSG(msg) \
    printf("Error : \"%s\", fail at %s:%d\n",(msg),__FILE__,__LINE__);


///////////////////////////////////////////////////////////////////////////////
//Global enum
typedef enum
{
    WBK_FAIL = 0,
    WBK_OK = 1
}WBK_RESULT;

struct WordBook;


///////////////////////////////////////////////////////////////////////////////
//Global interface declare
struct WordBook* 
WordBook_init(int nrOfMaxWords);

WBK_RESULT 
WordBook_fini(struct WordBook* pWordBook);

WBK_RESULT 
WordBook_AppendWord(
        struct WordBook* pWordBook, 
        const char* pWord
);

WBK_RESULT 
WordBook_GetWord(
        const struct WordBook* pWordBook, 
        int index,
        const char** pWord
);

#endif

