#ifndef SJTU_UTILITY_HPP
#define SJTU_UTILITY_HPP

#include <cstring>
#include <iostream>
#include "exceptions.hpp"
#include "constant.h"

namespace sjtu {

    template<class T>
    T min(T a, T b) {
        return a < b ? a : b;
    }

    template<class T>
    void swap(T &a, T &b) {
        T tmp = a;
        a = b;
        b = tmp;
    }

    template <class T, class Comp = std::less<T> >
    void sort(T _begin, T _end, Comp cmp)
    {
        if (_end - _begin <= 1)
            return;
        T _temp = _begin;
        T _front = _begin;
        T _back = _end - 1;
        while (true)
        {
            while (_back - _front >= 0)
            {
                if (cmp(*_back, *_temp))
                {
                    swap(*_back, *_temp);
                    _temp = _back;
                    _back--;
                    break;
                }
                _back--;
            }
            while (_back - _front >= 0)
            {
                if (cmp(*_temp, *_front))
                {
                    swap(*_temp, *_front);
                    _temp = _front;
                    _front++;
                    break;
                }
                _front++;
            }
            if (_back - _front < 0)
                break;
        }
        sort(_begin, _temp, cmp);
        sort(_temp + 1, _end, cmp);
    }

    bool isDigit(char ch) {
        return ch >= '0' && ch <= '9';
    }
}

namespace _string {

    template<int STRING_LEN = sjtu::DEFAULT_LEN>
    class String {
        friend std::ostream &operator<<(std::ostream &os, const String<STRING_LEN> &obj) {
            os << obj.data;
            return os;
        }
        friend std::istream &operator>>(std::istream &is, String<STRING_LEN> &obj) {
            is >> obj.data;
            obj.len = (int)strlen(obj.data);
            return is;
        }

    private:
        char data[STRING_LEN];
        int len;

    public:

        String(): len(0) {
            memset(data, 0, sizeof(data));
        }

        String(const char* ch){
            len = (int)strlen(ch);
            if(len >= STRING_LEN)
                throw sjtu::overflow();
            memcpy(data, ch, STRING_LEN);
            data[len] = 0;
        }


        String(const String &other):len(other.len) {
            memcpy(data, other.data, STRING_LEN);
            data[len] = 0;
        }

        String(String &&other) = default;

        String& operator=(const String &other) {
            if(this == &other)
                return *this;
            len = other.len;
            memcpy(data, other.data, STRING_LEN);
            data[len] = 0;
            return *this;
        }

        String& operator+=(const String &other) {
            if(len + other.len >= STRING_LEN)
                throw sjtu::overflow();
            for(int i = 0, j = len; i < other.len; i++, j++)
                data[j] = other[i];
            len += other.length();
            data[len] = 0;
            return *this;
        }

        String& operator+= (const char &ch) {
            if(len + 1 >= STRING_LEN)
                throw sjtu::overflow();
            data[len++] = ch;
            data[len] = 0;
            return *this;
        }

        bool operator== (const String &other) const{
            if(len != other.len)
                return false;
            for(int i = 0; i < len; i++)
                if(data[i] != other[i])
                    return false;
            return true;
        }

        bool operator!= (const String &other) const{
            return !this->operator==(other);
        }

        bool operator< (const String &other) const{
            int l = sjtu::min(len, other.len);
            for(int i = 0; i < l; i++)
                if(data[i] != other.data[i])
                    return data[i] < other.data[i];
            return len < other.len;
        }

        char& operator[] (const int &i) {
            if(i < 0 || i > len)
                throw sjtu::index_out_of_bound();
            return data[i];
        }

        char operator[] (const int &i) const {
            if(i < 0 || i > len)
                throw sjtu::index_out_of_bound();
            return data[i];
        }

        int length() const {
            return len;
        }

        String get(int l, int r) {
            String res;
            res.len = r - l + 1;
            for(int i = l; i <= r; i++)
                res[i - l] = data[i];
            res[res.len] = 0;
            return res;
        }
    };

}

namespace sjtu {

    typedef _string::String<> string;
    typedef _string::String<NAME_LEN> Lstring;

    double stringToDouble(string str) {
        int len = str.length(), pos = 0;
        double res = 0, base = 1;
        while(pos < len && !isDigit(str[pos]))
            pos++;
        while(pos < len && isDigit(str[pos]))
            res = res * 10 + str[pos] - '0', ++pos;
        ++pos;
        while(pos < len) {
            base /= 10;
            res += (str[pos] - '0') * base;
            ++pos;
        }
        return res;
    }

    template<class T1, class T2>
    class pair {
    public:
        T1 first;
        T2 second;
        constexpr pair() : first(), second() {}
        pair(const pair &other) = default;
        //pair(pair &&other) = default;
        pair(const T1 &x, const T2 &y) : first(x), second(y) {}
        template<class U1, class U2>
        pair(U1 &&x, U2 &&y) : first(x), second(y) {}
        template<class U1, class U2>
        pair(const pair<U1, U2> &other) : first(other.first), second(other.second) {}
        template<class U1, class U2>
        pair(pair<U1, U2> &&other) : first(other.first), second(other.second) {}

        bool operator<(const pair<T1, T2> &other) const{
            return (first < other.first) || (first == other.first && second < other.second);
        }

        bool operator==(const pair<T1, T2> &other) const{
            return first == other.first && second == other.second;
        }
    };

    class time {
    public:
        int hour, mini;
    public:

        time(): hour(-1), mini(-1) {}
        time(int h, int m): hour(h), mini(m) {}

        bool operator< (const time &other) const{
            return hour < other.hour || (hour == other.hour && mini < other.mini);
        }

        bool operator==(const time &other) const {
            return hour == other.hour && mini == other.mini;
        }

        bool operator<= (const time &other) const{
            this->operator==(other) || this->operator<(other);
        }

        time operator+ (const time &other) const{
            time res(*this);
            res += other;
            return res;
        }

        time operator- (const time &other) const{
            time res(*this);
            res -= other;
            return res;
        }

        time operator+= (const time &other) {
            hour += other.hour;
            mini += other.mini;
            hour += mini / 60;
            mini %= 60;
            return *this;
        }

        time operator-= (const time &other) {
            hour -= other.hour;
            mini -= other.mini;
            if(mini < 0) {
                hour--;
                mini += 60;
            }
            return *this;
        }

        string out() const{
            string res;
            if(hour == -1)
                return "xx:xx";
            if(hour < 10) {
                res += '0';
                res += hour + '0';
            }
            else {
                res += (hour / 10) + '0';
                res += (hour % 10) + '0';
            }
            res += ":";
            if(mini < 10) {
                res += '0';
                res += mini + '0';
            }
            else {
                res += (mini / 10) + '0';
                res += (mini % 10) + '0';
            }
            return res;
        }
    };

    std::ostream &operator<<(std::ostream &os, const time &obj) {
        if(obj.hour == -1) {
            os << "xx:xx";
        }
        else {
            if(obj.hour < 10)
                os << '0';
            os << obj.hour << ":";
            if(obj.mini < 10)
                os << '0';
            os << obj.mini;
        }
        return os;
    }

    std::istream &operator>>(std::istream &is, time &obj) {
        string tmp;
        is >> tmp;
        if(tmp[0] == 'x')
            obj.hour = obj.mini = -1;
        else {
            obj.hour = (int)stringToDouble(tmp.get(0, 1));
            obj.mini= (int)stringToDouble(tmp.get(3, 4));
        }
        return is;
    }

}

#endif
