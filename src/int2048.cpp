//
// Created by Lynn on 2021/11/12.
//

#include "int2048.h"

namespace BigInt {

    #define MAXN 300000
    #define BASE 10000

    const double Pi = acos(-1.0) ;

    int decimal_digit[8] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000};
    int temporary_rev[MAXN];
    lint temporary_lint[MAXN], temporary_buffer[MAXN];
    int sum = 0;

    complex temporary_x[MAXN], temporary_y[MAXN], temporary_z[MAXN], omega[MAXN];
    int2048 temporary_a, temporary_b, TA, TB, TC;

    void init(int l) {
        int n = 1 << l;
        for (int i = 1; i <= n; i <<= 1)
            for (int j = 0; j < i; ++j) {
                double deg = 2 * Pi * j / i;
                omega[i | j] = complex(cos(deg), sin(deg));
            }
    }

    void butterfly(complex *const a, const int m, const int k) {
        complex *const b = a + m;
        complex *const omg = omega + k;
        for (int i = 0; i < m; i++) {
            complex &x = a[i], &y = b[i];
            const complex tmp = y * omg[i];
            y = x - tmp;
            x = x + tmp;
        }
    }

    void FFT(complex* a, int l, int type) {
        const int n = 1 << l, size = 1 << (l < 16 ? l : 16);
        for (int i = 0, j = 0; i < n; i++) {
            if (i < j)
                std::swap(a[i], a[j]);
            for (int l = n >> 1; (j ^= l) < l; l >>= 1);
        }
        for (int i = 0; i < n; i += size)
            for (int m = 1, k = 2; m < size; m <<= 1, k <<= 1)
                for (int j = 0; j < size; j += k)
                    butterfly(a + i + j, m, k);
        for (int m = size, k = size << 1; m < n; m <<= 1, k <<= 1)
            for (int j = 0; j < n; j += k)
                butterfly(a + j, m, k);
        if (!~type) {
            for (int i = 1; i <= (n >> 1); i++)
                std::swap(a[i], a[n - i]);
            for (int i = 0; i < n; i++)
                a[i] = a[i] * (1.0 / n);
        }
    }

    void int2048::shift(int k) {
        if (k > 0) {
            a.resize(n + k);
            for (int i = n + k - 1; i >= k; --i)
                a[i] = a[i - k];
            for (int i = k - 1; i >= 0; --i)
                a[i] = 0;
            n += k;
        }
        else {
            k = -k;
            if (k > n) {
                for (int i = 0; i < n; ++i)
                    a[i] = 0;
                n = 1;
                a.resize(1);
            }
            else {
                for (int i = 0; i < n - k; ++i)
                    a[i] = a[i + k];
                for (int i = n - k; i < n; ++i)
                    a[i] = 0;
                n = n - k;
                a.resize(n);
            }
        }
    }

    void int2048::setzero() {
        if (n == 1 && a[0] == 0)
            sign = 1;
    }

    int limit(int n) {
        int m = 0;
        for (int i = 1; i < n; i <<= 1)
            m++;
        return m;
    }

    bool smaller(const int2048 & a, const int2048 & b) {
        if (a.n != b.n)
            return a.n < b.n;
        for (int i = a.n - 1; i >= 0; --i)
            if (a.a[i] != b.a[i])
                return a.a[i] < b.a[i];
        return false;
    }

    void output(lint x) {
        if (x < 0) {
            x = -x;
            putchar('-');
        }
        if (x > 9)
            output(x / 10);
        putchar(x % 10 + '0');
    }

    int calculatedigit(lint x) {
        int digit = 0;
        while (x) {
            x /= 10;
            ++digit;
        }
        if (!digit)
            digit = 1;
        return digit;
    }

    int2048::int2048() {
        a.clear();
        n = 1;
        a.push_back(0);
        sign = 1;
    }

    int2048::int2048(lint v) {
        n = 0;
        sign = (v < 0 ? -1 : 1);
        v *= sign;
        a.clear();
        while (v) {
            a.push_back(v % BASE);
            v /= BASE;
        }
        n = a.size();
        if (!n) {
            n = 1;
            a.push_back(0);
        }
    }

    int2048::int2048(const std::string &s) {
        read(s);
    }

    int2048::int2048(const int2048 &b) {
        n = b.n;
        sign = b.sign;
        a = b.a;
    }

    void int2048::read(const std::string &s) {
        a.clear();
        n = 0;
        sign = (s[0] == '-' ? -1 : 1);
        int current_value = 0;
        for (int i = s.size() - 1, j = 0; i >= (s[0] == '-' ? 1 : 0); --i, ++j) {
            current_value += (s[i] - '0') * decimal_digit[j];
            if (j == 3) {
                j = -1;
                a.push_back(current_value);
                current_value = 0;
            }
        }
        if (current_value)
            a.push_back(current_value);
        n = a.size();
        if (!n) {
            n = 1;
            a.push_back(0);
        }
        setzero();
    }

    void int2048::print() const {
        std::cout << *this;
    }

    int int2048::toInt() const {
        int limit = std::min(1, n - 1);
        int res = 0;
        for (int i = limit; i >= 0; --i)
            res = res * BASE + a[i];
        return res;
    }

    lint int2048::toLongLong() const {
        int limit = std::min(3, n - 1);
        lint res = 0;
        for (int i = limit; i >= 0; --i)
            res = res * BASE + a[i];
        return res;
    }

    double int2048::toDouble() const {
        double res = 0;
        for (int i = n - 1; i >= 0; --i)
            res = res * BASE + a[i];
        return res;
    }

    std::string int2048::toString() const {
        std::string res = "";
        if (sign < 0)
            res = res + "-";
        res = res + std::to_string(a[n - 1]);
        for (int i = n - 2; i >= 0; --i) {
            int digit = calculatedigit(a[i]);
            for (int j = 1; j <= 4 - digit; ++j)
                res = res + "0";
            res = res + std::to_string(a[i]);
        }
        return res;
    }

    int2048 ADD(const int2048 &a, const int2048 &b) { // ignore signal
        int2048 c;
        c.sign = a.sign;
        c.n = std::max(a.n, b.n);
        c.a = a.a;
        c.a.resize(c.n);
        for (int i = 0; i < b.n; ++i)
            if ((c.a[i] += b.a[i]) >= BASE) {
                c.a[i] -= BASE;
                if (i == c.n - 1)
                    c.a.push_back(0);
                ++c.a[i + 1];
            }
        c.n = c.a.size();
        return c;
    }

    int2048 MINUS(const int2048 &a, const int2048 &b) { // absolutely a > b, ignore signal
        int2048 c(a);
        c.sign = 1;
        for (int i = 0; i < std::min(a.n, b.n); ++i) {
            c.a[i] -= b.a[i];
            if (c.a[i] < 0) {
                c.a[i] += BASE;
                --c.a[i + 1];
            }
        }
        for (int i = std::min(a.n, b.n); i < a.n; ++i)
            if (c.a[i] < 0) {
                c.a[i] += BASE;
                --c.a[i + 1];
            }
            else
                break;
        while (!c.a.empty() && !c.a.back())
            c.a.pop_back();
        if (c.a.empty())
            c.a.push_back(0);
        c.n = c.a.size();
        return c;
    }

    int2048 operator + (const int2048 &a, const int2048 &b) {
        if (a.sign * b.sign < 0) {
            int2048 c;
            if (smaller(a, b)) {
                c = MINUS(b, a);
                c.sign = b.sign;
            }
            else {
                c = MINUS(a, b);
                c.sign = a.sign;
            }
            c.setzero();
            return c;
        }
        else
            return ADD(a, b);
    }

    int2048 &int2048::operator += (const int2048 &b) {
        *this = *this + b;
        return *this;
    }

    int2048 operator - (const int2048 &a, const int2048 &b) {
        int2048 c;
        if (a.sign * b.sign < 0) {
            c = ADD(a, b);
            if (b.sign < 0)
                c.sign = 1;
            else
                c.sign = -1;
        }
        else {
            if (smaller(a, b)) {
                c = MINUS(b, a);
                c.sign = (b.sign > 0 ? -1 : 1);
            }
            else {
                c = MINUS(a, b);
                c.sign = (a.sign > 0 ? 1 : -1);
            }
        }
        c.setzero();
        return c;
    }

    int2048 &int2048::operator -= (const int2048 &b) {
        *this = *this - b;
        return *this;
    }

    void MUL(const int2048 &a, const int2048 &b, int2048 &c) {
        int l = limit(a.n + b.n), n = (1 << l);
        init(l);
        for (int i = 0; i < n; ++i)
            temporary_z[i].x = temporary_z[i].y = 0;
        for (int i = 0; i < a.n; ++i)
            temporary_z[i].x = a.a[i];
        for (int i = 0; i < b.n; ++i)
            temporary_z[i].y = b.a[i];
        FFT(temporary_z, l, 1);
        temporary_z[n] = temporary_z[0];
        for (int i = 0; i < n; ++i) {
            double tempa = temporary_z[i].x, tempb = temporary_z[i].y, tempc = temporary_z[n - i].x, tempd = -temporary_z[n - i].y;
            temporary_x[i] = complex((tempa + tempc) / 2, (tempb + tempd) / 2);
            temporary_y[i] = complex((tempb - tempd) / 2, (tempc - tempa) / 2);
        }
        for (int i = 0; i < n; ++i)
            temporary_x[i] = temporary_x[i] * temporary_y[i];
        FFT(temporary_x, l, -1);
        c.n = n;
        c.sign = a.sign * b.sign;
        for (int i = 0; i < c.n; ++i)
            temporary_lint[i] = 0;
        for (int i = 0; i < c.n; ++i) {
            temporary_lint[i] += (lint)(temporary_x[i].x + 0.5);
            if (temporary_lint[i] >= BASE) {
                temporary_lint[i + 1] = temporary_lint[i] / BASE;
                temporary_lint[i] %= BASE;
                c.n += (i == (c.n - 1));
            }
        }
        while (!temporary_lint[c.n - 1] && c.n > 0)
            --c.n;
        c.a.resize(c.n);
        for (int i = 0; i < c.n; ++i)
            c.a[i] = temporary_lint[i];
        c.setzero();
    }
    int2048 operator * (const int2048 &a, const int2048 &b) {
        int2048 c;
        MUL(a, b, c);
        return c;
    }

    int2048 &int2048::operator *= (const int2048 &b) {
        *this = *this * b;
        return *this;
    }

    void downplay(int M, int m) {
        if (m == 1) {
            int val = BASE * BASE / int(temporary_b.a[temporary_b.n - 1]);
            TA.a.resize(2);
            TA.sign = 1;
            TA.n = 2;
            TA.a[0] = val % BASE;
            TA.a[1] = val / BASE;
            return ;
        }
        if (m == 2) {
            lint val = 1ll * BASE * BASE * BASE * BASE / lint(temporary_b.a[temporary_b.n - 1] * BASE + temporary_b.a[temporary_b.n - 2]);
            TA.a.resize(0);
            TA.n = 0;
            TA.sign = 1;
            while (val) {
                TA.a.push_back(val % BASE);
                val /= BASE;
                ++TA.n;
            }
            return ;
        }
        int k = m / 2 + 1;
        downplay(M, k);
        TB = TA;
        TC = temporary_b;
        TC.shift(-M + m);
        TA = TA * int2048(2);
        TA.shift(m - k);
        TB = TB * TB * TC;
        TB.shift(-2 * k);
        TA = TA - TB;
        TB = int2048(1);
        TB.shift(2 * m);
    }

    int2048 operator / (const int2048 &a, const int2048 &b) {
        int n = a.n, m = b.n;
        temporary_a = a;
        temporary_b = b;
        temporary_a.sign = temporary_b.sign = 1;
        if (n > (m << 1)) {
            int k = n - (m << 1);
            temporary_a.shift(k);
            temporary_b.shift(k);
            n += k;
            m += k;
        }
        downplay(m, m);
        TA = temporary_a * TA;
        TA.sign = a.sign * b.sign;
        TA.shift(-2 * m);
        if (!TA.n) {
            TA.n = 1;
            TA.a.clear();
            TA.a.push_back(0);
        }
        if (b.sign > 0) {
            while ((TA + int2048(1)) * b <= a)
                TA += int2048(1);
            while (TA * b > a)
                TA -= int2048(1);
        }
        else {
            while (TA * b < a)
                TA -= int2048(1);
            while ((TA + int2048(1)) * b >= a)
                TA += int2048(1);
        }
        TA.setzero();
        return TA;
    }

    int2048 &int2048::operator /= (const int2048 & b) {
        *this = *this / b;
        return *this;
    }

    bool operator == (const int2048 &a, const int2048& b) {
        if (a.sign != b.sign)
            return false;
        if (a.n != b.n)
            return false;
        for (int i = 0; i < a.n; ++i)
            if (a.a[i] != b.a[i])
                return false;
        return true;
    }

    bool operator != (const int2048 &a, const int2048& b) {
        return !(a == b);
    }

    bool operator < (const int2048 &a, const int2048 &b) {
        if (a.sign != b.sign)
            return a.sign < b.sign;
        return a.sign == 1 ? smaller(a, b) : smaller(b, a);
    }

    bool operator <= (const int2048 &a, const int2048 &b) {
        if (a.sign != b.sign)
            return a.sign < b.sign;
        if (a.n != b.n)
            return a.sign == 1 ? a.n < b.n : a.n > b.n;
        for (int i = a.n - 1; i >= 0; --i)
            if (a.a[i] != b.a[i])
                return a.sign == 1 ? a.a[i] < b.a[i] : a.a[i] > b.a[i];
        return true;
    }

    bool operator > (const int2048 &a, const int2048 &b) {
        return !(a <= b);
    }

    bool operator >= (const int2048 &a, const int2048 &b) {
        return !(a < b);
    }

    std::istream &operator >> (std::istream &isinput, int2048 &a) {
        std::string temp;
        isinput >> temp;
        a.read(temp);
        return isinput;
    }

    std::ostream &operator << (std::ostream &osoutput, const int2048 &a) {
        if (a.sign < 0)
            putchar('-');
        output(a.a[a.n - 1]);
        for (int i = a.n - 2; i >= 0; --i) {
            int digit = calculatedigit(a.a[i]);
            for (int j = 1; j <= 4 - digit; ++j)
                putchar('0');
            output(a.a[i]);
        }
        return osoutput;
    }

    int2048 operator % (const int2048 &a, const int2048 &b) {
//        if (a >= b)
//            return int2048();
        return a - a / b * b;
    }

    int2048 &int2048::operator %= (const int2048 &b) {
        *this = *this % b;
        return *this;
    }


}