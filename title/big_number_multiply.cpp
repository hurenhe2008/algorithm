#include <stdlib.h>
#include <string.h>

/*
@func
	big_number_multiply - multiply two unsigned number.
@params
	m - the first unsigned number
	n - the second unsigned number
@return 
	the result of two unsigned number multiply.
*/
unsigned long long big_number_multiply(unsigned m, unsigned n)
{
    unsigned long long ret = 0;
    unsigned big_num = (m > n) ? m : n;
    unsigned min_num = (m > n) ? n : m;
    unsigned i = 0;

    for (i = 0; i < 32; i++) {
        if ((min_num >> i) & 0x01) {
            ret += ((unsigned long long)big_num << i);
        }
        //the bit number is 0, not handle
    }
    return ret;
}

/*
@func
	big_number_multiply_x - multiply two big long number string.
@params
	m - the first number string
	n - the second number string
@return 
	the result number string after multiply.
*/
const char* big_number_multiply_x(const char *m, const char *n)
{
    char *str = nullptr;
    unsigned *num = nullptr;
    int mlen = strlen(m), nlen = strlen(n);
    int len = (mlen + nlen) * sizeof(unsigned);
    int hightpos = mlen + nlen - 1;
    int i = 0, j = 0;

    if (!m || !n || !len) {
        return nullptr;
    }

    num = (unsigned *)malloc(len);
    memset(num, 0, len);

    for (i = 0; i < mlen; ++i) {
        for (j = 0; j < nlen; ++j) {
            num[i + j] += (m[mlen - 1 - i] - '0') * (n[nlen - 1 - j] - '0');
        }
    }

    for (i = 0; i < (mlen + nlen - 1); ++i) {
        num[i + 1] += num[i] / 10;
        num[i] = num[i] % 10;
    }

    /*locate hight bit position of num*/
    for (i = mlen + nlen - 1; i >= 0; --i) {
        if (num[i]) {
            hightpos = i;
            break;
        }
    }

    str = (char *)malloc(sizeof(char) * (hightpos + 2));
    for (i = hightpos; i >= 0; --i) {
        str[i] = num[hightpos - i] + '0';
    }
    str[hightpos + 1] = '\0';

    free(num);
    return str;
}