/**
 * 实现一个最小堆
 * huchenyuan
 *      0    -   sentinel element
 *      1    -   min element
 *    2   3
 *   4 5 6 7
 **/
#include "minheap.h"

int minheap_insert(struct minheap *h, int64_t pri, void *data)
{
    int i;
    if (pri <= MIN_PRI)
        return -2;
    if (h->count >= h->capacity)
        return -1;
    h->count++;
    for (i = h->count; h->node[i / 2].pri > pri; i /= 2) {
        h->node[i] = h->node[i / 2];
    }
    h->node[i].pri = pri;
    h->node[i].data = data;
    return 0;
}

int minheap_delete_id(struct minheap *h, unsigned int id)
{
    struct minheap_node last;
    int i;
    int child;
    if (h->count == 0 || id == 0)
        return -1;

    last = h->node[h->count];
    h->count--;

    for (i = id; i * 2 <= h->count; i = child) {
        child = i * 2;
        if (child != h->count && (h->node[child + 1].pri < h->node[child].pri))
            child++;

        if (last.pri > h->node[child].pri) {
            h->node[i] = h->node[child];
        } else {
            break;
        }
    }
    h->node[i] = last;
    return 0;
}
