#ifndef VECTOR_H
#define VECTOR_H

#include <cstdlib>
#include <iostream>
#include "constant.h"

namespace sjtu {

    template <class ElemType>
    class vector {
    public:
        int len;
        ElemType vec[BlockSize * 2 / sizeof(ElemType)];


        explicit vector(const int &&size = 0) {
            len = 0;
        }
        ~vector() {
        }
        void push_back(const ElemType &elem) {
            vec[len++] = elem;
        }
        void insert(const int &pos, const ElemType &elem) {

            for (int i = len; i > pos; i--){vec[i] = vec[i - 1];}
            vec[pos] = elem;
            ++len;
        }
        void erase(const int &pos) {
            if(pos >= len) throw(sjtu::index_out_of_bound::exception());
            for(int i = pos; i < len - 1; i++){vec[i] = vec[i+1];}
            --len;
        }
        void pop_back() {
            if(len == 0) throw(sjtu::index_out_of_bound::exception());
            --len;
        }
        void clear() {
            len = 0;
        }
        short size() const {
            return (short)len;
        }
        ElemType &back() {
            if(len == 0) throw(sjtu::index_out_of_bound::exception());
            return vec[len-1];
        }
        ElemType &front() {
            if(len == 0) throw(sjtu::index_out_of_bound::exception());
            return vec[0];
        }
        vector<ElemType> &operator= (const vector<ElemType> &other) {
            if(this == &other) return *this;
            len = other.len;
            for(int i = 0; i < len; ++i)
                vec[i] = other.vec[i];
            return *this;
        }
        ElemType& operator[] (const short &pos) {
            return vec[pos];
        }
        ElemType operator[] (const short &pos) const{
            return vec[pos];
        }
        void spilt(const vector<ElemType> &other, const short &left, const short &right) {
            for(int i = left; i < right; i++) {
                vec[i-left] = other.vec[i];
            }
            len = right - left;
        }
        void change_len(short newLen) {
            len = newLen;
        }
    };
};

#endif
