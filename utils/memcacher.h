#ifndef MEMCACHER_H_D4ED3D51_2486_11E0_AC09_005056C00008__INCLUDE_
#define MEMCACHER_H_D4ED3D51_2486_11E0_AC09_005056C00008__INCLUDE_
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
typedef enum
{
    MC_FAIL = 0,
    MC_OK   = 1
}MC;

typedef struct
{
    unsigned char* pBuffer;
    long           nrOfBytes;
}MCBLOB;

typedef struct MEMORYCACHER MEMORYCACHER;

/**
 * @brief initial cache.
 */
MEMORYCACHER* memcacher_init(int maxNumberOfItem);

/**
 * @brief free all dynamic data.
 */
MC memcacher_fini(MEMORYCACHER* pMemoryCacher);

/**
 * @brief push pBLOB to cache
 * @tips the pBLOB->pBuffer will direct save, 
 *      don't free pBLOB->pBuffer your self.
 */
MC memcacher_push(MEMORYCACHER* pMemoryCacher, long bookIndex, MCBLOB* pBLOB);

/**
 * @brief try to get back data.
 */
MC memcacher_pull(MEMORYCACHER* pMemoryCacher, long bookIndex, const MCBLOB** pBLOB);

/**
 * @brief free data.
 */
MC memcacher_erase(MEMORYCACHER* pMemoryCacher, long bookIndex);

#endif

