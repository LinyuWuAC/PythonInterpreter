//
// Created by Lynn on 2021/11/3.
//

#ifndef BIGNUMBER_INT2048_H
#define BIGNUMBER_INT2048_H

#include <iostream>
#include <cstring>
#include <cstdio>
#include <vector>
#include <complex>
#include "int2048.h"

namespace BigInt {

    typedef long long lint;
    typedef unsigned long long ulint;

    struct complex {
        double x, y;
        complex (double x = 0, double y = 0):x(x), y(y) {}
        complex operator + (const complex a) const {
            return complex(x + a.x, y + a.y);
        }
        complex operator - (const complex a) const {
            return complex(x - a.x, y - a.y);
        }

        complex operator * (const complex a) const {
            return complex(x * a.x - y * a.y, x * a.y + y * a.x);
        }
    };

    class int2048 {
    private:
        std::vector<int> a;
        int n;
        int sign;
        friend bool smaller(const int2048 &, const int2048 &);
        void setzero();
    public:
        int2048();
        int2048(lint v);
        int2048(const std::string &);
        int2048(const int2048 &);

        void read(const std::string &);
        void print() const;

        void shift(int);
        friend void downplay(int, int);
        int toInt() const;
        lint toLongLong() const;
        double toDouble() const;
        std::string toString() const;

        friend int2048 ADD(const int2048 &, const int2048 &);
        friend int2048 MINUS(const int2048 &, const int2048 &);

        friend int2048 operator + (const int2048 &, const int2048 &);
        int2048 &operator += (const int2048 &);

        friend int2048 operator - (const int2048 &, const int2048 &);
        int2048 &operator -= (const int2048 &);

        friend void MUL(const int2048 &, const int2048 &, int2048 &);
        friend int2048 operator * (const int2048 &, const int2048 &);
        int2048 &operator *= (const int2048 &);

        friend int2048 operator / (const int2048 &, const int2048 &);
        int2048 &operator /= (const int2048 &);

        friend int2048 operator % (const int2048 &, const int2048 &);
        int2048 &operator %= (const int2048 &);

        friend std::istream &operator >> (std::istream &, int2048 &);
        friend std::ostream &operator << (std::ostream &, const int2048 &);

        friend bool operator == (const int2048 &, const int2048 &);
        friend bool operator != (const int2048 &, const int2048 &);
        friend bool operator < (const int2048 &, const int2048 &);
        friend bool operator > (const int2048 &, const int2048 &);
        friend bool operator <= (const int2048 &, const int2048 &);
        friend bool operator >= (const int2048 &, const int2048 &);
    };

}

#endif //BIGNUMBER_INT2048_H