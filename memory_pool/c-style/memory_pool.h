#ifndef __MEMORY_POOL_H__
#define __MEMORY_POOL_H__


/**************************************************************
@memory pool:
    if size > 128
        directly call malloc to allocate memory.
    if size <= 128 
        check adapted block from block list array.

@note:
    all memory alignment with 8 byte (fastly locate address),
    block list array with 8、16、32、64、128 byte block chain,
    every block chain is a list chain.

    at first, create a big memory pool chunk for allocation.
    if one block list is nullptr at begin, allocate 20 node,
    if the big memory pool has not enough size, first 
***************************************************************/


/***
@function  
        allocate()
@param
        size: allocate memory size
@return
        return allocated memory address, nullptr if error
***/
void* allocate(unsigned size);

/***
@function
    dealloate()
@param
    pdata: memory address from allocate()'s return 
    size: pdata memory block size, not neccessary.
@return
    no return
***/
void dealloate(void *pdata, unsigned size);


#endif //__MEMORY_POOL_H__
