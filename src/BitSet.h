/*************************************************
 ** Copyright(c) 2018, luzeya
 ** All rights reserved
 **
 ** 文件名：BitSet.h
 ** 创建人：zeya
 ** 创建日期： 2018-07-29
 **
 ** 描  述：位图结构的集合
 ** 注  意：只适合顺序的
 **************************************************/
#ifndef BITSET_H
#define BITSET_H

#include <cstring>

class BitSet
{
public:

    explicit BitSet(long num)
        : num(num),
          data(0) {
        long n = cal();
        data = new char[n];
        memset(data, 0, n);
    }

    ~BitSet() {
        delete[] data;
    }

    long cal() {
        if ((num % 8) == 0) {
            return num >> 3;
        }
        return (num >> 3) + 1;
    }

    void set(long n) {
        if (n >= num)
            return;
        ((((char*)(data))[(n) >> 3]) |= (1 << ((n) & 0x7)));
    }

    void unset(long n) {
        if (n >= num)
            return;
        ((((char*)(data))[(n) >> 3]) &= (~(1 << ((n) & 0x7))));
    }

    bool check(long n) {
        if (n >= num)
            return false;
        return ((((char*)(data))[(n) >> 3]) & (1 << ((n) & 0x7)));
    }

    bool operator==(BitSet &set) {
        if (num != set.num)
            return false;
        return 0 == memcmp(data, set.data, num);
    }

private:

    long num;

    char *data;
};

#endif /* BITSET_H */
