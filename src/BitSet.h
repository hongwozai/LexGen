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
        bytes = cal();
        data = new char[bytes];
        memset(data, 0, bytes);
    }

    ~BitSet() {
        delete[] data;
    }

    long cal() {
        if ((num & 7) == 0) {
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

    void clear() {
        memset(data, 0, bytes);
    }

    bool operator==(BitSet &set) {
        if (num != set.num)
            return false;
        return 0 == memcmp(data, set.data, num);
    }

    bool operator<(BitSet &set) {
        if (num != set.num) {
            return false;
        }

        for (long i = bytes - 1; i >= 0; i--) {
            if (data[i] < set.data[i]) return true;
        }
        return false;
    }

    int move(BitSet &set) {
        delete[] data;
        this->num   = set.num;
        this->bytes = set.bytes;
        this->data  = set.data;
        set.num   = 0;
        set.bytes = 0;
        set.data  = 0;
        return -1;
    }

public:

    long num;

    long bytes;

    char *data;
};

class PtrBitSet
{
public:
    bool operator()(BitSet *b1, BitSet *b2) const {
        return *b1 == *b2;
    }
};

#endif /* BITSET_H */
