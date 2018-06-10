#ifndef __VARIADIC_TEMPLATES__H__
#define __VARIADIC_TEMPLATES__H__

#include <iostream>

namespace sjtu
{
    template <class T1, class T2, class T3>
    class tuple3 {
        friend std::ostream& operator<< (std::ostream &os, const tuple3<T1, T2, T3> &obj);
        friend std::istream& operator>> (std::istream &is, tuple3<T1, T2, T3> &obj);

    public:
        T1 data1;
        T2 data2;
        T3 data3;

        tuple3() {}
        tuple3(const T1& a, const T2& b, const T3& c):data1(a), data2(b), data3(c) {}
        tuple3(const tuple3& other):data1(other.data1), data2(other.data2), data3(other.data3) {}

        bool operator==(const tuple3& other) const{
            return data1 == other.data1 && data2 == other.data2 && data3 == other.data3;
        }
        bool operator<(const tuple3& other) const{
            if(data1 != other.data1)
                return data1 < other.data1;
            if(data2 != other.data2)
                return data2 < other.data2;
            return data3 < other.data3;
        }

        T1 head() {
            return data1;
        }
    };

    template <class T1, class T2, class T3>
    std::ostream &operator<<(std::ostream &os, const tuple3<T1, T2, T3> &obj) {
        os << obj.data1 << ' ' << obj.data2 << ' ' << obj.data3;
        return os;
    }

    template <class T1, class T2, class T3>
    std::istream &operator>>(std::istream &is, tuple3<T1, T2, T3> &obj) {
        is >> obj.data1 >> obj.data2 >> obj.data3;
        return is;
    }

    template <class T1, class T2, class T3, class T4, class T5, class T6>
    class tuple6 {
        friend std::ostream& operator<< (std::ostream &os, const tuple6<T1, T2, T3, T4, T5, T6> &obj);
        friend std::istream& operator>> (std::istream &is, tuple6<T1, T2, T3, T4, T5, T6> &obj);

    public:
        T1 data1;
        T2 data2;
        T3 data3;
        T4 data4;
        T5 data5;
        T6 data6;

        tuple6() {}
        tuple6(const T1& a, const T2& b, const T3& c, const T4& d, const T5& e, const T6& f):
                data1(a), data2(b), data3(c), data4(d), data5(e), data6(f) {}
        tuple6(const tuple6& other):
                data1(other.data1), data2(other.data2), data3(other.data3), data4(other.data4), data5(other.data5), data6(other.data6) {}

        bool operator==(const tuple6& other) const{
            return data1 == other.data1 && data2 == other.data2 && data3 == other.data3 && data4 == other.data4
                   && data5 == other.data5 && data6 == other.data6;
        }
        bool operator<(const tuple6& other) const{
            if(data1 != other.data1)
                return data1 < other.data1;
            if(data2 != other.data2)
                return data2 < other.data2;
            if(data3 != other.data3)
             return data3 < other.data3;
            if(data4 != other.data4)
                return data4 < other.data4;
            if(data5 != other.data5)
                return data5 < other.data5;
            return data6 < other.data6;
        }

        T1 head() {
            return data1;
        }
    };

    template <class T1, class T2, class T3, class T4, class T5, class T6>
    std::istream &operator>>(std::istream &is, tuple6<T1, T2, T3, T4, T5, T6> &obj) {
        is >> obj.data1 >> obj.data2 >> obj.data3 >> obj.data4 >> obj.data5 >> obj.data6;
        return is;
    }

    template <class T1, class T2, class T3, class T4, class T5, class T6>
    std::ostream &operator<<(std::ostream &os, const tuple6<T1, T2, T3, T4, T5, T6> &obj) {
        os << obj.data1 << ' ' << obj.data2 << ' ' << obj.data3 << ' ' << obj.data4 << ' ' << obj.data5 << ' ' << obj.data6;
        return os;
    }

    // =============== tuple 存储不同类型的数据 ======================//
    template<typename... TList> class tuple;

    template<> class tuple<>{
    public:
        bool operator<(const tuple<>& other) {
            return false;
        }
    };

    template<typename T, typename... TList>
    class tuple<T, TList...>
            : private tuple<TList...>
    {
    public:
        typedef T value_type;
        typedef tuple<TList...> base_type;
        typedef tuple<T, TList...> this_type;

    public:
        tuple() {}
        tuple(const T& v, const TList&... tails) : base_type(tails...), _value(v) {}
        tuple(const T&& v, const TList&&... tails) : base_type(std::forward<const TList>(tails)...), _value(v) {}

        /*
        // 本来是想使用下面的两个构造函数, 但是测试的时候发现有bug
        // 在递归构造的时候, 第一次触发构造方式第一参数是右值以后, 后面的都会进入第一参数是右值的构造
        // 最后只能要不全是右值的, 要不就全不是右值 来处理
        tuple(const T& v, const TList&... tails)
            : base_type(std::forward<const TList>(tails)...)
            , _value(v)
        {
            cout << "tuple(const T& v, const TList&... tails) " << v << endl;
        }

        tuple(const T&& v, const TList&... tails)
            : base_type(std::forward<const TList>(tails)...)
            , _value(v)
        {
            cout << "tuple(const T&& v, const TList&... tails) " << v << endl;
        }
        */

        tuple(const this_type& other) :base_type(static_cast<const base_type&>(other)), _value(other._value) {}

        tuple(const this_type&& other) :base_type(static_cast<const base_type&&>(other)), _value(other._value) {}

        const T& head() const { return _value; }
        T& head() { return this->_value; }

        this_type& operator = (const this_type& other)
        {
            base_type::operator = (static_cast<const base_type&>(other));
            _value = other._value;
            return *this;
        }

        this_type& operator = (this_type&& other)
        {
            base_type::operator = (std::move(static_cast<base_type>(other)));
            _value = other._value;
            return *this;
        }
        /*
        bool operator<(const this_type& other) {
            if(_value != other._value)
                return _value < other._value;
            return base_type::operator<(static_cast<const base_type&>(other));
        }*/

    protected:
        T _value;
    };

    // =============== 获取第N个数据的类型 ======================//
    template<unsigned int N, typename... TList> struct tuple_at;

    template<unsigned int N, typename T, typename... TList>
    struct tuple_at< N, tuple<T, TList...> >
    {
        typedef typename tuple_at< N - 1, tuple<TList...> >::value_type value_type;
        typedef typename tuple_at< N - 1, tuple<TList...> >::tuple_type tuple_type;
    };

    template<typename T, typename... TList>
    struct tuple_at< 0, tuple<T, TList...> >
    {
        typedef T value_type;
        typedef tuple<T, TList...> tuple_type;
    };


    // =============== tuple_get: 取出第N个数据 ======================//
    template<unsigned int N, typename... TList>
    const typename tuple_at<N, tuple<TList...> >::value_type&
    tuple_get(const tuple<TList...>& tp)
    {
        typedef tuple<TList...> tuple_type;
        typedef typename tuple_at<N, tuple_type>::tuple_type base_tuple_type;

        return static_cast<const base_tuple_type&>(tp).head();
    }

    template<unsigned int N, typename... TList>
    typename tuple_at<N, tuple<TList...> >::value_type&
    tuple_get(tuple<TList...>& tuple_)
    {
        typedef tuple<TList...> tuple_type;
        typedef typename tuple_at<N, tuple_type >::tuple_type base_tuple_type;

        return static_cast<base_tuple_type&>(tuple_).head();
    }

    // =============== makeTuple: 创建tuple ======================//
    template<typename... Type>
    tuple<Type...> makeTuple(const Type&... args)
    {
        return tuple<Type...>(args...);
    }

    template<typename... Type>
    tuple<Type...> makeTuple(const Type&&... args)
    {
        return tuple<Type...>(std::forward<const Type>(args)...);
    }
}

#endif