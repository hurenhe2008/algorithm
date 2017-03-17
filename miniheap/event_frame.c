/**
 * 一个简单的事件框架
 * 目标是应付fd和timer都极少的应用场景
 * author: huchenyuan
 * 2015/08/30
 **/
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include "event_frame.h"
#include "minheap.h"

/* 判断bit是否设置 */
#define TEST_BIT(x, bit)  ((x & bit) != 0)

struct ef_fd_node {
    int fd;
    int event;
    ef_fd_event_cb_t cb;
    void *data;

    struct ef_fd_node *next;
};

struct ef {
    struct minheap      *hp_timer;

    struct ef_fd_node *fd_list;

    struct ef_idle_task *idle_list;
};

static struct ef s_ef;

/* return ms from cpu up */
static inline int64_t __mt_now()
{
    struct timespec spec;
    int ret;
    ret = clock_gettime(CLOCK_MONOTONIC, &spec);
    assert(ret == 0);

    return spec.tv_sec * 1000UL + spec.tv_nsec / 1000000UL;
}

struct ef_timer* event_frame_add_timer(int timeout_ms,
        ef_timer_cb_t cb, void *data)
{
    int ret;
    struct ef_timer *timer;
    if ((timer = malloc(sizeof(struct ef_timer))) == NULL)
        return NULL;

    timer->wake_time = __mt_now() + timeout_ms;
    timer->cb = cb;
    timer->data = data;
    ret = minheap_insert(s_ef.hp_timer, timer->wake_time, timer);
    if (ret != 0) {
        free(timer);
        return NULL;
    }
    return timer;
}

int event_frame_del_timer(struct ef_timer *timer)
{
    int ret, id;

    id = minheap_find(s_ef.hp_timer, timer);
    if (id <= 0)
        return -1;
    ret = minheap_delete_id(s_ef.hp_timer, id);
    assert(ret == 0);

    free(timer);
    return 0;
}

int event_frame_add_fd(int fd, int event, ef_fd_event_cb_t cb, void *data)
{
    struct ef_fd_node *node;
    node = malloc(sizeof(*node));
    if (node == NULL)
        return -1;

    node->fd = fd;
    node->event = event;
    node->cb = cb;
    node->data = data;

    /* 单链表添加 */
    node->next = s_ef.fd_list;
    s_ef.fd_list = node;

    return 0;
}

int event_frame_del_fd(int fd, int event)
{
    struct ef_fd_node *node, **head;

    /* 单链表删除 */
    for (head = &s_ef.fd_list; *head;) {
        node = *head;
        if (node->fd == fd && node->event == event) {
            /* find */
            *head = node->next;
            free(node);
            return 0;
        }
        head = &node->next;
    }

    return -1;
}

struct ef_idle_task* event_frame_add_idle_task(ef_idle_task_cb_t cb,
        void *data)
{
    struct ef_idle_task *task;
    task = malloc(sizeof(*task));
    if (task == NULL)
        return NULL;

    task->cb = cb;
    task->data = data;
    /* 单链表添加 */
    task->next = s_ef.idle_list;
    s_ef.idle_list = task;

    return task;
}

int event_frame_del_idle_task(struct ef_idle_task *task)
{
    struct ef_idle_task *node, **head;

    /* 单链表删除 */
    for (head = &s_ef.idle_list; *head;) {
        node = *head;
        if (node == task) {
            /* find */
            *head = node->next;
            free(node);
            return 0;
        }
        head = &node->next;
    }

    return -1;
}

static int event_frame_timer_do(struct timeval *ttval, int64_t now)
{
    struct minheap_node *hp_node;
    struct ef_timer *timer;
    int64_t timeout; /* ms */
    int has_timer;

    /* 处理所有定时器的超时 */
    has_timer = 0;
    timeout = 0;                                        
    while ((hp_node = minheap_peek_min(s_ef.hp_timer)) != NULL) {
        timeout = hp_node->pri - now;  /* ms */
        if (timeout > 0) {
            has_timer = 1;
            ttval->tv_sec = timeout / 1000;
            ttval->tv_usec = (timeout % 1000) * 1000;
            break;
        }
        timer = hp_node->data;
        minheap_delete_min(s_ef.hp_timer);

        timer->cb(timer->data);
        free(timer);
    }
    return has_timer;
}

static void event_frame_idle_task_do(int64_t now)
{
    struct ef_idle_task *node;

    for (node = s_ef.idle_list; node; node = node->next) {
        node->cb(now, node->data);
    }
}

int event_frame_loop()
{
    fd_set read_fds;
    fd_set write_fds;
    fd_set except_fds;
    struct timeval ttval;
    int has_timer;
    int ret;
    struct ef_fd_node *node;
    int maxfd;
    int64_t now;

    while (1) {
        now = __mt_now(); /* ms */
        has_timer = event_frame_timer_do(&ttval, now);

        event_frame_idle_task_do(now);

        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        FD_ZERO(&except_fds);
        maxfd = 0;

        for (node = s_ef.fd_list; node; node = node->next) {
            if (node->fd > maxfd)
                maxfd = node->fd;
            if (node->event == EF_EVENT_READ) {
                FD_SET(node->fd, &read_fds);
            } else if (node->event == EF_EVENT_WRITE) {
                FD_SET(node->fd, &write_fds);
            } else {
                FD_SET(node->fd, &except_fds);
            }
        }

        ret = select(maxfd + 1, &read_fds, &write_fds, &except_fds,
                has_timer ? &ttval : NULL);

        if (ret == 0) {
            continue;
        }
        if (ret < 0) {
            break;
        }

        for (node = s_ef.fd_list; node; node = node->next) {
            if (node->event == EF_EVENT_READ) {
                if (FD_ISSET(node->fd, &read_fds)) {
                    node->cb(node->fd, node->event, node->data);
                }
            } else if (node->event == EF_EVENT_WRITE) {
                if (FD_ISSET(node->fd, &write_fds)) {
                    node->cb(node->fd, node->event, node->data);
                }
            } else {
                if (FD_ISSET(node->fd, &except_fds)) {
                    node->cb(node->fd, node->event, node->data);
                }
            }
        }
    }
    return ret;
}

int event_frame_init()
{
    s_ef.hp_timer = malloc(MINHEAP_MEM_SIZE(EF_MAX_TIMER));
    if (s_ef.hp_timer == NULL)
        return -1;
    minheap_init(s_ef.hp_timer, EF_MAX_TIMER);
    s_ef.fd_list = NULL;
    return 0;
}
