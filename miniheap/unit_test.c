/**
 * 简单的单元测试
 * author: huchenyuan
 **/
#ifdef __EVENT_FRAME_UNIT_TEST__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "event_frame.h"

#define CHECK_0(x) do { \
    if (x) { \
        printf("error %d\n", __LINE__); \
        exit(-1); \
    } \
} while (0)

static struct ef_timer *s_timer;

static void fd_cb(int fd, int event, void *data)
{
    char buff[128];
    int ret;
    memset(buff, 0, sizeof(buff));
    ret = read(fd, buff, sizeof(buff));
    printf("fd=%d, event=%d, %p. (%d, %s)\n", fd, event, data, ret, buff);
}

static void timer_cb(void *data)
{
    printf("timer %p\n", data);
    s_timer = event_frame_add_timer(1000, timer_cb, (void *)0xf1);
}

int main()
{
    int ret;
    event_frame_init();

    s_timer = event_frame_add_timer(100, timer_cb, (void *)0xf0);
    ret = event_frame_del_timer(s_timer);
    CHECK_0(ret);

    s_timer = event_frame_add_timer(1000, timer_cb, (void *)0xf1);

    ret = event_frame_add_fd(STDIN_FILENO, EF_EVENT_READ, fd_cb, (void *)0xc0);
    CHECK_0(ret);

    ret = event_frame_loop();
    CHECK_0(ret);
    return 0;
}

#endif /* __EVENT_FRAME_UNIT_TEST__ */
