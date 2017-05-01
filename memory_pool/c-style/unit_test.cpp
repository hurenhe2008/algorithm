#include "memory_pool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_STRING "hello, world 1234567890-=qwertyuiopasdfghjkllzxcvbnm,./[]saldfjslafwerweroosdfnn,sndfwqerqwer;ljlsdf!"

int main(int argc, const char *argv[])
{
    for (unsigned i = 1; i < 10; ++i) {
        unsigned size = 1 << i;
        char *tmp = (char *)allocate(size);

        if (!tmp) {
            abort();
        }

        memcpy_s(tmp, size - 1, TEST_STRING, size - 1);
        tmp[size - 1] = '\0';

        printf("size:%d  string:%s\n", size, tmp);

        dealloate((void *)tmp, size);
    }

    for (unsigned i = 1; i < 10; ++i) {

        unsigned size = 1 << i;
        char *tmp[25] = { 0 };
        for (unsigned j = 0; j < 25; ++j) {    //25 > 20
            tmp[j] = (char *)allocate(size);

            if (!tmp[j]) {
                abort();
            }

            memcpy_s(tmp[j], size - 1, TEST_STRING, size - 1);
            tmp[j][size - 1] = '\0';
            printf("size:%d  string:%s\n", size, tmp[j]);
        }

        for (unsigned j = 0; j < 25; ++j) {
            dealloate((void *)tmp[j], size);
        }
    }

    getchar();

    return 0;
}