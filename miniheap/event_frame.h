/**
 * 一个简单的事件框架
 * 目标是应付fd和timer都极少的应用场景
 * author: huchenyuan
 * 2015/08/30
 **/
#ifndef __EVENT_FRAME_H__
#define __EVENT_FRAME_H__

#include <stdio.h>
#include <stdint.h>


typedef void (*ef_timer_cb_t)(void *data);
typedef void (*ef_fd_event_cb_t)(int fd, int event, void *data);
typedef void (*ef_idle_task_cb_t)(int64_t now /* ms */, void *data);

struct ef_timer {
    int64_t wake_time;
    ef_timer_cb_t cb;
    void *data;
};

struct ef_idle_task {
    ef_idle_task_cb_t cb;
    void *data;
    struct ef_idle_task *next;
};

/* 支持的最大定时器数 */
#define EF_MAX_TIMER 32

struct ef_timer* event_frame_add_timer(int timeout_ms,
        ef_timer_cb_t cb, void *data);

int event_frame_del_timer(struct ef_timer *timer);

#define EF_EVENT_WRITE  (1)
#define EF_EVENT_READ   (2)
#define EF_EVENT_EXPECT (3)
/**
 * 同一个fd，监听不同事件，可以调用几次
 * 但是同一个fd，同一个事件，反复添加其行为不可预期
 **/
int event_frame_add_fd(int fd, int event, ef_fd_event_cb_t cb, void *data);
int event_frame_del_fd(int fd, int event);

/**
 * 添加一个idle task，在每次陷入睡眠之前，都会执行一次
 * 返回task_id
 * 小于0失败
 **/
struct ef_idle_task* event_frame_add_idle_task(ef_idle_task_cb_t cb,
        void *data);
int event_frame_del_idle_task(struct ef_idle_task *task);

int event_frame_loop();

int event_frame_init();

#endif /* __EVENT_FRAME_H__ */
