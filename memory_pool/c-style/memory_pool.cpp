#include "memory_pool.h"
#include <cstdlib>
#include <mutex>

//alignment size
#define ALIGN_BASE             8
//align m to 8's double number
#define ALIGN_RET(m)          ((m + (ALIGN_BASE - 1)) & (~(ALIGN_BASE - 1)))

//block list count
#define BLOCK_LIST_COUNT       5   
//every block list max node
#define BLOCK_LIST_MAX_NODES   20
//block node max size 
#define BLOCK_NODE_MAX_SIZE    128

//record a block information
#pragma pack(4)
typedef struct node_s {
    //next node
    struct node_s *next;
    //node size
    unsigned char size;   /* can disable size */
} node_t;
#pragma pack()

//block list array
//index 0:  8 byte block list   (8 * 2^0)
//index 1:  16 byte block list  (8 * 2^1)
//index 2:  32 byte block list  (8 * 2^2)
//index 3:  64 byte block list  (8 * 2^3)
//index 4:  128 byte block list (8 * 2^4)
static node_t* s_block_list[BLOCK_LIST_COUNT] = { 0 };

//memory allocate mutex
static std::mutex s_mem_mutex;

//memory pool
static void* s_mem_pool = nullptr;
static const unsigned s_pool_size = 104857600;   //100 * 1024 * 1024
static unsigned s_pool_cur_pos = 0;
static unsigned s_mem_alloc_count = 0; 

//initialize memory pool
static inline void init_mem_pool();

//allocate block list from memory pool
static bool allocate_block_list(unsigned char index, unsigned nodesize, unsigned count);

//allocate block node from block list
static node_t* allocate_block_node(unsigned char index);

//recycle block node to block list
static bool recycle_block_node(unsigned char index, node_t *node);

//calculate block list index in array with align size
static inline unsigned char calc_array_index(unsigned nodesize);


void* allocate(unsigned size)
{
    if (size > BLOCK_NODE_MAX_SIZE) {
        return (void *)malloc(size);
    }

    void *tmp = nullptr;
    unsigned char align_size = ALIGN_RET(size);
    unsigned char index = calc_array_index(align_size);

    s_mem_mutex.lock();

    do {
        node_t *head = allocate_block_node(index);
        if (head) {
            tmp = head;
            break;
        }

        //try allocate from big table if block list is empty.
        if (allocate_block_list(index, align_size, BLOCK_LIST_MAX_NODES)) {
            if (head = allocate_block_node(index)) {
                tmp = head;
                break;
            }
        }

        //try allocate from bigger block list and put left to block list.
        //this will lead to bigger block list less to none.
        //how to compact smaller block list nodes to bigger block list node.
        if (!head) {
            
        }

        //no memory can allocate 
    } while (0);
   
    s_mem_mutex.unlock();
    
    return tmp;
}

void dealloate(void *pdata, unsigned size)
{
    if (size > BLOCK_NODE_MAX_SIZE) {
        free(pdata);
        return;
    }

    unsigned char align_size = ALIGN_RET(size);
    unsigned char index = calc_array_index(align_size);

    node_t *node = (node_t *)pdata;
    node->size = align_size;
    node->next = nullptr;

    recycle_block_node(index, node);
}

void init_mem_pool()
{
    if (s_mem_pool) {
        return;
    }

    s_mem_pool = (void *)malloc(s_pool_size);

    if (!s_mem_pool) {
        abort();
    }

    s_pool_cur_pos = 0;
    s_mem_alloc_count = 0;
}

bool allocate_block_list(unsigned char index, unsigned nodesize, unsigned count)
{
    if (!s_mem_pool) {
        init_mem_pool();
    }

    unsigned needsize = nodesize * count;
    unsigned leftsize = s_pool_size - s_pool_cur_pos;

    //no enough memory to allocate
    if (leftsize < needsize) {
        return nullptr;
    }

    char *head = (char *)s_mem_pool + s_pool_cur_pos;
    char *start = head;
    node_t *tmp = nullptr;

    for (unsigned i = 0; i < (count - 1); ++i) {    
        tmp = (node_t *)start;
        tmp->next = (node_t *)(start + nodesize);
        tmp->size = nodesize;
        start = start + nodesize;
    }

    //last node's next is nullptr
    tmp = (node_t *)start;
    tmp->next = nullptr;
    tmp->size = nodesize;

    s_block_list[index] = (node_t *)head;

    s_pool_cur_pos = s_pool_cur_pos + needsize;
    return true;
}

node_t* allocate_block_node(unsigned char index) 
{
    if (index >= BLOCK_LIST_COUNT) {
        abort();
    }

    node_t *tmp = s_block_list[index];

    if (tmp) {
        s_block_list[index] = tmp->next;
        ++s_mem_alloc_count;
    }

    return tmp;
}

bool recycle_block_node(unsigned char index, node_t *node)
{
    if (index >= BLOCK_LIST_COUNT) {
        abort();
    }

    if (!node) {
        return false;
    }

    node->next = s_block_list[index];
    s_block_list[index] = node;
    --s_mem_alloc_count;

    return true;
}

unsigned char calc_array_index(unsigned nodesize)
{
    unsigned char i = 0;

    while(i < BLOCK_LIST_COUNT) {
        if ((nodesize >> (3 + i)) & 0x01) {
            break;
        }
        ++i;
    }

    if (i >= BLOCK_LIST_COUNT) {
        abort();
    }

    return i;
}