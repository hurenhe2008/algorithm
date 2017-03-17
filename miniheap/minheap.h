/**
 * min heap
 *     -1    -   sentinel element
 *      1    -   min element
 *    2   3
 *   4 5 6 7
 *
 * auth: HuChenyuan <hcyjob@gmail.com>
 **/
#ifndef __MINHEAP_H__
#define __MINHEAP_H__

#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* sentinel element value */
#define MIN_PRI   (-1)

struct minheap_node {
    int64_t pri; /* priority */
    void *data;
};

struct minheap {
    unsigned int count;
    unsigned int capacity;
    struct minheap_node node[0];
};

#define MINHEAP_MEM_SIZE(capacity) \
    (sizeof(struct minheap) + ((capacity)+1) * sizeof(struct minheap_node))

static inline void minheap_init(struct minheap *h, unsigned int capacity)
{
    h->count = 0;
    h->capacity = capacity;
    h->node[0].data = NULL;
    h->node[0].pri = MIN_PRI; /* sentinel element */
}

static inline struct minheap_node* minheap_peek_id(struct minheap *h, int id)
{
    if (h->count >= id)
        return &h->node[id];
    return NULL;
}

static inline struct minheap_node* minheap_peek_min(struct minheap *h)
{
    return minheap_peek_id(h, 1);
}

int minheap_insert(struct minheap *h, int64_t pri, void *data);

int minheap_delete_id(struct minheap *h, unsigned int id);

static inline int minheap_delete_min(struct minheap *h)
{
    return minheap_delete_id(h, 1);
}

static inline int minheap_find(struct minheap *h, void *data)
{
    int i;
    for (i = 1; i <= h->count; i++) {
        if (h->node[i].data == data)
            return i;
    }
    return -1;
}

#endif /* __MINHEAP_H__ */
