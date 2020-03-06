/*
Copyright (c) 2018 Paul Stahr

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef UTIL_H
#define UTIL_H

#include <algorithm>
#include <stdexcept>
#include <ostream>
#include <iterator>
#include <typeinfo>
#include <iterator>
#include <functional>
#include <sstream>
#include <iostream>
#include <deque>
#include <cassert>
#include <cmath>
#include <vector>
#include <valarray>
#include <type_traits>
#include "iterator_util.h"

#ifdef NDEBUG
#define only_debug(code) ((void)0)
#else
#define only_debug(code) code
#endif

template<typename T>
struct halftype_struct{typedef void _type;};
template <> struct halftype_struct<uint64_t>{typedef uint32_t _type;};
template <> struct halftype_struct<uint32_t>{typedef uint16_t _type;};
template <> struct halftype_struct<uint16_t>{typedef uint8_t _type;};
template <> struct halftype_struct<int64_t> {typedef int32_t _type;};
template <> struct halftype_struct<int32_t> {typedef int16_t _type;};
template <> struct halftype_struct<int16_t> {typedef int8_t _type;};
template <> struct halftype_struct<double>  {typedef float _type;};
template <typename T>
using halftype = typename halftype_struct<T>::_type;


template<typename T>
struct unsignedtype_struct{typedef void type;};
template <> struct unsignedtype_struct<int64_t>{typedef uint64_t _type;};
template <> struct unsignedtype_struct<int32_t>{typedef uint32_t _type;};
template <> struct unsignedtype_struct<int16_t>{typedef uint16_t _type;};
template <> struct unsignedtype_struct<int8_t>{typedef uint8_t _type;};
template <> struct unsignedtype_struct<uint64_t>{typedef uint64_t _type;};
template <> struct unsignedtype_struct<uint32_t>{typedef uint32_t _type;};
template <> struct unsignedtype_struct<uint16_t>{typedef uint16_t _type;};
template <> struct unsignedtype_struct<uint8_t>{typedef uint8_t _type;};
template <> struct unsignedtype_struct<double>  {typedef double _type;};
template <> struct unsignedtype_struct<float>  {typedef float _type;};
template <typename T>
using unsignedtype = typename unsignedtype_struct<T>::_type;


struct pair_id_injection{
    typedef halftype<size_t> element_type;
    typedef size_t id_type;
    typedef std::array<element_type, 2> pair_type;
    std::vector<pair_type> _id_to_pair;
    element_type _num_elements;
    id_type _num_ids;
    std::vector<id_type> _pair_to_id_data;
    
    template <typename BinaryPredicate>
    pair_id_injection(element_type n, BinaryPredicate exists) : _num_elements(n), _num_ids((n * (n - 1)) / 2), _pair_to_id_data(n * n, std::numeric_limits<size_t>::max())
    {
        _id_to_pair.reserve(_num_ids);
        for (element_type i = 0; i < n; ++i)
        {
            for (element_type j = i + 1; j < n; ++j)
            {
                if (exists(i, j))
                {
                    _pair_to_id_data[i * n + j] = _pair_to_id_data[j * n + i] = _id_to_pair.size();
                    _id_to_pair.push_back(std::array<element_type, 2>({{i,j}}));
                }
            }
        }
    }

    id_type operator ()(element_type i, element_type j) const;

    std::array<element_type, 2> const & operator [](id_type i) const;

    std::vector<size_t>::const_iterator get_row(element_type row) const;

    pair_id_injection(element_type n);
};

template <typename T, typename V>
V convert_to(T const & elem)
{
    return static_cast<V>(elem);
}

class NullBuffer : public std::streambuf
{
public:
  int overflow(int c);
};

class NullStream : public std::ostream {
    public: 
       NullStream();
    private:
        NullBuffer m_sb;
};

namespace UTIL
{   
    template <class T, T V>
    struct template_constant
    {
        template_constant(){};    
        constexpr operator T() const { return V; }
        template <typename W>
        constexpr T operator ()(W const &){return V;}
        template <typename W, typename X>
        constexpr T operator ()(W const &, X const &){return V;}
    };
    
    static template_constant<bool, true> const logical_true;
    static template_constant<bool, false> const logical_false;
    
    template <typename T>
    bool isnan(T t)
    {
        return std::isnan(t);
    }
    
    template <typename T>
    int8_t sign(T x)
    {
        return x > 0 ? 1 : (x < 0 ? -1 : 0);
    }

    template <typename T>
    int8_t sign(T x, T y)
    {
        return x > y ? 1 : (x < y ? -1 : 0);
    }
    
    struct abs_struct
    {
        template <typename T>
        unsignedtype<T> operator()(T x) const
        {
            return x >= 0 ? x : -x;
        }
    };
    
    static const abs_struct abs;
    
    template <typename T>
    struct dummy_function{
        T _t;
        dummy_function(T const & t_) : _t(t_){}

        T const & operator()(){return _t;}
        template <typename V>
        T const & operator()(V const &){return _t;}
    };
      
    template <typename T>
    dummy_function<T> get_dummy_function(T const & t)
    {
        return dummy_function<T>(t);
    }
    
    template <typename T>
    struct max_struct{
        max_struct(){}
        T const & operator()(T const & x, T const & y) const{return x < y ? y : x;}
    };
    
    template <typename T>
    static const max_struct<T> max;

    template <typename T>
    struct min_struct{
        min_struct(){}
        T const & operator()(T const & x, T const & y) const{return x > y ? y : x;}
    };
    
    template <typename T>
    static const min_struct<T> min;
    
    /*template <typename T, typename V>
    struct square_bracket_struct{
        square_bracket_struct(){}
        typename std::result_of<T::operator[](V)>::type const & operator()(T const & x, V const & index) const{return x[index];}
    };*/
    
    template <typename T>
    struct plus_clamp_funct
    {
        T operator()(T const & left, T const & right) const
        {
            if (left < 0)
            {
                return std::numeric_limits<T>::min() - left > right ?  std::numeric_limits<T>::min() : left + right;
            }
            return std::numeric_limits<T>::max() - left < right ?  std::numeric_limits<T>::max() : left + right;
        }
    };

    template <typename T, typename F>
    struct insert_left_operator_struct : std::unary_function<T, T>
    {
        T _comp;
        F _func;
        insert_left_operator_struct(T const & comp_, F const & func_): _comp(comp_), _func(func_){}
        typename std::result_of<F(T,T)>::type operator()(T const & arg) const{return _func(_comp, arg);}
    };
    
    template <typename T, typename F>
    struct insert_right_operator_struct
    {
        T _comp;
        F _func;
        insert_right_operator_struct(T const & comp_, F const & func_): _comp(comp_), _func(func_){}
        template <typename U>
        typename std::result_of<F(U,T)>::type operator()(U const & arg) const{return _func(arg, _comp);}
    };
    
    struct shift_right_struct
    {
        template <typename T>
        T operator()(T const & lhs, uint8_t rhs) const{return lhs >> rhs;}
    };
    
    template <typename T, typename F>
    insert_left_operator_struct<T, F> get_insert_left_operator(T const & comp, F const & func){return insert_left_operator_struct<T, F>(comp, func);}

    template <typename T, typename F>
    insert_right_operator_struct<T, F> get_insert_right_operator(T const & comp, F const & func){return insert_right_operator_struct<T, F>(comp, func);}
    
    insert_right_operator_struct<uint8_t, shift_right_struct> shift_right(uint8_t comp);

    template <typename T>insert_right_operator_struct<T, std::divides<T> >  divide       (T const & comp){return get_insert_right_operator(comp, std::divides<T>());}
    template <typename T>insert_left_operator_struct<T, std::divides<T> >   divide_by    (T const & comp){return get_insert_left_operator (comp, std::divides<T>());}
    template <typename T>insert_right_operator_struct<T, std::minus<T> >    minus        (T const & comp){return get_insert_right_operator(comp, std::minus<T>());}
    template <typename T>insert_left_operator_struct<T, std::multiplies<T> >multiply     (T const & comp){return get_insert_left_operator (comp, std::multiplies<T>());}
    template <typename T>insert_right_operator_struct<T, std::less<T> >     less         (T const & comp){return get_insert_right_operator(comp, std::less<T>());}
    template <typename T>insert_right_operator_struct<T, std::greater<T> >  greater      (T const & comp){return get_insert_right_operator(comp, std::greater<T>());}
    template <typename T>insert_right_operator_struct<T, max_struct<T> >    max_inserted (T const & comp){return get_insert_right_operator(comp, max<T>);}
    template <typename T>insert_right_operator_struct<T, min_struct<T> >    min_inserted (T const & comp){return get_insert_right_operator(comp, min<T>);}
    template <typename T>insert_left_operator_struct<T, std::plus<T> >      plus         (T const & comp){return get_insert_left_operator (comp, std::plus<T>());}
    template <typename T>insert_left_operator_struct<T, std::equal_to<T> >  equal_to     (T const & comp){return get_insert_left_operator (comp, std::equal_to<T>());}
    template <typename T>insert_left_operator_struct<T, plus_clamp_funct<T> > plus_clamp (T const & comp){return get_insert_left_operator (comp, plus_clamp_funct<T>());}
    
    struct add_to_struct
    {
        add_to_struct(){}
        template <typename T>
        T operator()(T & lhs, T const & rhs) const{return lhs += rhs;}
    };

    static const add_to_struct add_to;
    
    struct mult_by_struct
    {
        mult_by_struct(){}
        template <typename T>
        T operator()(T & lhs, T const & rhs) const{return lhs *= rhs;}
    };

    static const mult_by_struct mult_by;
    
    struct divide_by_struct
    {
        divide_by_struct(){}
        template <typename T>
        T operator()(T & lhs, T const & rhs) const{return lhs /= rhs;}
    };

    static const divide_by_struct divid_by;
    
    struct subtract_from_struct
    {
        subtract_from_struct(){}
        template <typename T>
        T operator()(T & lhs, T const & rhs) const{return lhs -= rhs;}
    };

    static const subtract_from_struct subtract_from;
    
    template <class InputIter, class Func>
    class transformation_iterator
    {
    protected:
        InputIter _iter;
        Func _f;
        
    public:
        transformation_iterator(InputIter iter_, const Func & f_): _iter(iter_), _f(f_){}
        
        typename std::result_of<Func(typename ITER_UTIL::it_value_type<InputIter>::elem)>::type operator*() const
        { 
            return _f(*_iter);
        }
        
        template <typename T>
        typename std::result_of<Func(typename ITER_UTIL::it_value_type<InputIter>::elem)>::type operator[](T index) const
        { 
            return _f(_iter[index]);
        }
        
        transformation_iterator& operator++(){++_iter; return *this;}  
        transformation_iterator operator++(int){transformation_iterator tmp(*this); ++_iter; return tmp;}

        transformation_iterator& operator--(){--_iter; return *this;}  
        transformation_iterator operator--(int){transformation_iterator tmp(*this); --_iter; return tmp;}
        
        transformation_iterator operator+(typename std::iterator_traits<InputIter>::difference_type diff){transformation_iterator tmp(*this); tmp += diff; return tmp;}
        transformation_iterator& operator+=(typename std::iterator_traits<InputIter>::difference_type diff){_iter += diff; return *this;}
        
        bool operator !=(transformation_iterator<InputIter, Func> & other)const{return _iter != other._iter;}
    };
    
    template <class InputIter, class Func2>
    inline transformation_iterator<InputIter, Func2> transform_iter(const InputIter& iter, const Func2& g)
    {
        return transformation_iterator<InputIter,Func2>(iter, g); 
    }
    
    template <class Func1, class Func2>
    class compose_functor
    {
    protected:
        Func1 f;
        Func2 g;
    
    public:
        
        compose_functor(const Func1& _f, const Func2& _g) : f(_f), g(_g) { }
        
        template <typename T>
        typename std::result_of<Func1(typename std::result_of<Func2(T)>::type)>::type operator()(T const & x) const
        { 
            return f(g(x));
        }
    };
    
    template <typename InputIter0, typename InputIter1, typename BinaryPredicate>
    bool all_of (InputIter0 begin0, InputIter0 end0, InputIter1 begin1, BinaryPredicate pred)
    {
        for (;begin0 != end0;++begin0, ++begin1)
        {
            if (!pred(*begin0, *begin1))
            {
                return false;
            }
        }
        return true;
    }
    
    template<class InputIterator, class InputIterator2, class BinaryPredicate>
    InputIterator find_if (InputIterator first, InputIterator last, InputIterator2 first2, BinaryPredicate pred)
    {
        while (first!=last) {
            if (pred(*first, *first2)) return first;
            ++first;
            ++first2;
        }
        return last;
    }
    
    template< class InputIt, class InputIt2, class BinaryPredicate >
    constexpr bool any_of(InputIt first, InputIt last, InputIt2 first2, BinaryPredicate p)
    {
        return find_if(first, last, first2, p) != last;
    }
    
    template <class Func1, class Func2>
    inline compose_functor<Func1, Func2> compose(const Func1& f, const Func2& g)
    {
        return compose_functor<Func1,Func2>(f, g); 
    }
    
    bool endsWith(const std::string& str, const std::string& suffix);

    bool startsWith(const std::string& str, const std::string& prefix);

    template <typename InputIterator, typename OutputIterator>
    void copy(InputIterator ibegin, InputIterator iend, OutputIterator obegin, OutputIterator oend)
    {
        std::copy_n(ibegin, std::min(std::distance(ibegin, iend), std::distance(obegin, oend)), obegin);
    }
    
    template <typename InputIter, typename OutputIter, typename IndexIter>
    void copy_elements(InputIter ibegin, OutputIter obegin, IndexIter begin, IndexIter end)
    {
        while (begin != end)
        {
            obegin[*begin] = ibegin[*begin];
            ++begin;
        }
    }
    
    template <class ForwardIterator, class T>
    void copy_indices(T begin, T end, ForwardIterator out)
    {
        while (begin != end)
        {
            *out = begin;
            ++begin;
        }
    }
    
    template <typename T>
    T divide_round_up(T value, uint64_t div)
    {
        if (std::is_integral<T>::value)
        {
            return (value + (div - 1)) / div;
        }
        return value / div;
    }
    
    template<class ForwardIterator, class T>
    void iota_n(ForwardIterator first, size_t count, T value)
    {
        while (count --> 0) {
            *first = value;
            ++first;
            ++value;
        }
    }
    
    template<class InputIt, class UnaryPredicate>
    InputIt find_if_n(InputIt first, InputIt last, UnaryPredicate p)
    {
        size_t index = 0;
        for (; first != last; ++first, ++index) {
            if (p(index)) {
                return first;
            }
        }
        return last;
    }
    
    template<class ForwardIt, class UnaryPredicate>
    ForwardIt remove_if_n(ForwardIt first, ForwardIt last, UnaryPredicate p)
    {
        ForwardIt tmp = first;
        first = find_if_n(first, last, p);
        if (first != last)
        {
            size_t index = std::distance(tmp, first) + 1;
            for(ForwardIt i = first; ++i != last; ++index)
            {
                if (!p(index))
                {
                    *first = std::move(*i);
                    ++first;
                }
            }
        }
        return first;
    }
    
    struct is_null_pointer_struct
    {
        is_null_pointer_struct(){}
        template <typename T>
        bool operator()(T const * ptr) const
        {
            return ptr == nullptr;
        }
    };
    
    static const is_null_pointer_struct is_null_pointer;
    
    struct to_string_struct
    {
        to_string_struct(){}
        template <typename T>
        std::string operator()(T const & t) const
        {
            return std::to_string(t);
        }
    };
    
    static const to_string_struct to_string;
    
    struct vector_comparator_struct
    {
        vector_comparator_struct(){}
        template <typename T>
        bool operator()(std::vector<T> const & left, std::vector<T> const & right) const
        {
            return std::lexicographical_compare(left.begin(), left.end(), right.begin(), right.end());
        }
    };
    
    static const vector_comparator_struct vector_comparator;

    template <typename T>
    void transpose(std::vector<std::vector<T> > const & in, std::vector<std::vector<T> > & out)
    {
        out.clear();
        size_t width = in.size();
        if (width == 0)
        {
            return;
        }
        size_t height = in[0].size();
        for (size_t i = 0; i < height; ++i)
        {
            out.emplace_back();
            out.back().reserve(width);
            for (size_t j = 0; j < width; ++j)
            {
                out.back().emplace_back(in[j][i]);
            }
        }
    }
    
    /**
     * @brief sets first[i] = first[from_index[i]]
     * 
     * @param first ...
     * @param last ...
     * @param from_index ...
     * @return void
     */
    template <class InputIt, class IndexIt>
    void permutate_from_iter(InputIt first, InputIt last, IndexIt from_index);
    
    void rand_mark(std::vector<bool> & take, size_t num, size_t seed);
    
    template <class InputIter0, class InputIter1>
    bool are_inverse(InputIter0 begin0, InputIter0 end0, InputIter1 begin1)
    {
        for (size_t i = 0; begin0 != end0; ++i)
        {
            if (begin1[*begin0] != i)
            {
                return false;
            }
            ++begin0;
        }
        return true;
    }
    
    template <typename T>
    halftype<T> sqrt_lower_bound(T value)
    {
        halftype<T> erg = 0;
        while (static_cast<T>(erg) * static_cast<T>(erg) <= value)
        {
            ++erg;
        }
        return erg - 1;
    }
    
    template <typename T>
    halftype<T> sqrt_upper_bound(T value)
    {
        halftype<T> erg = 0;
        while (static_cast<T>(erg) * static_cast<T>(erg) < value)
        {
            ++erg;
        }
        return erg;
    }
    
    template <class InputIterator, class OutputIterator, class BinaryIterator>
    OutputIterator copy_if_index (InputIterator first, InputIterator last,
                            OutputIterator result, BinaryIterator pred)
    {
        size_t index = 0;
        while (first!=last) 
        {
            if (pred[index]) 
            {
                *result = *first;
                ++result;
            }
            ++first;
            ++index;
        }
        return result;
    }
    
    template <typename T>
    void swap_indices(std::vector<std::vector<T> > const & in, std::vector<std::vector<T> > & out)
    {
        out.clear();
        if (in.size() == 0)
        {
            return;
        }
        size_t width = in.size(), height = in[0].size();
        out.resize(height, std::vector<T>());
        for (size_t i = 0; i < height; ++i)
        {
            std::vector<T> & current = out[i];
            current.reserve(width);
            for (size_t j = 0; j < width; ++j)
            {
                current.push_back(in[j][i]);
            }
        }
    }
    
    
    /**
     * @brief assign out[i] := value for i in begin to end if pred[i] is true
     * 
     * @param begin Beginning Indices
     * @param end End of Indices
     * @param out Iterator to write
     * @param value Value to write
     * @param pred Only write if pred[*IndexIter] is true
     * @return void
     */
    template <typename IndexIter, typename OutputIter, typename T, typename UnaryPredicate>
    void assign_elements_if(IndexIter begin, IndexIter end, OutputIter out, T value, UnaryPredicate pred)
    {
        for (;begin != end;++begin)
        {
            if (pred(*begin))
            {
                out[*begin] = value;
            }
        }
    }
    
    /*
     * assign out[i] := value for i in begin to end
     * 
     */
    template <typename IndexIter, typename OutputIter, typename T>
    void assign_elements(IndexIter begin, IndexIter end, OutputIter out, T value)
    {
        assign_elements_if(begin, end, out, value, UTIL::logical_true);
    }
    
    template <typename T, typename OutputIter, typename Function>
    void transform_counter(T begin, T end, OutputIter out, Function func)
    {
        while (begin != end)
        {
            out = func(begin);
            ++begin;
        }
    }
    
    struct identity_function_struct{
        identity_function_struct(){}
        
        template<typename U>
        constexpr auto operator()(U&& v) const noexcept
            -> decltype(std::forward<U>(v))
        {
            return std::forward<U>(v);
        }
    };
    
    static const identity_function_struct identity_function;

    template<class InputIterator, class Function>
    Function for_each_pair(
        InputIterator first0,
        InputIterator last0,
        InputIterator first1,
        InputIterator last1,
        Function fn)
    {
        while (first0!=last0) 
        {
            for (InputIterator iter = first1; iter != last1; ++iter)
            {
                fn (*first0, *iter);
            }
            ++first0;
        }
        return std::move(fn);
    }

    /*
     * Calls fn for every pair (x,y) it first to last
     */
    template<class InputIterator, class Function>
    Function for_each_pair(InputIterator first, InputIterator last, Function fn)
    {
        for (InputIterator iter0 = first; iter0 != last; ++iter0)
        {
            for (InputIterator iter1 = first; iter1 != last; ++iter1)
            {
                fn (*iter0, *iter1);
            }
        }
        return std::move(fn);
    }

    /*
     * Calls fn for every pair {x,y} in first to last
     */
    template<class InputIterator, class Function>
    Function for_each_pair_unordered(InputIterator first, InputIterator last, Function fn)
    {
        while (first!=last) 
        {
            for (InputIterator iter = first + 1; iter != last; ++iter)
            {
                fn (*first, *iter);
            }
            ++first;
        }
        return std::move(fn);
    }
    
    /*
     * Sets out[first[i]] := in[i]
     */
    template <class InputIt1, class InputIt2, class OutputIt> 
    void permutate_to_indice(InputIt1 first, InputIt1 last, InputIt2 in, OutputIt out)
    {
        for (;first != last;++first, ++in)
        {
            *(out + *first) = *in;
        }
    }

    /*
     * Sets out[i] := in[first[i]]
     * Can be used with a back inserter
     */
    template <class InputIt1, class InputIt2, class OutputIt> 
    void permutate_from_indice(InputIt1 first, InputIt1 last, InputIt2 in, OutputIt out)
    {
        for (;first != last;++first,++out)
        {
            *out = in[*first];
        }
    }

    /*
     * Creates inverse permutation, with
     * obegin[begin[i]] = i
     */
    template <class InputIt, class OutputIt>
    void permutate_inverse(InputIt begin, InputIt end, OutputIt obegin)
    {
        for (size_t i = 0;begin != end;++i, ++begin)
        {
            obegin[*begin] = i;
        }
    }
    
    template <typename InputIter>
    bool is_permutation(InputIter begin, InputIter end)
    {
        std::vector<bool> visited(std::distance(begin, end));
        while (begin != end)
        {
            if (visited[*begin])
            {
                return false;
            }
            visited[*begin] = true;
        }
        return true;
    }

    /*
     * sets begin[i] = begin[from_index[i]]
     */
    template <class InputIt, class IndexIt>
    void permutate_from_iter(InputIt begin, InputIt end, IndexIt from_index)
    {
        size_t distance = std::distance(begin, end);
        std::vector<bool> visited(distance, false);
        for (size_t i = 0; i < distance; ++i)
        {
            if (visited[i])
            {
                continue;
            }
            size_t last = i;
            size_t next = i;
            auto tmp = begin[i];
            while ((next = from_index[next]) != i)
            {
                assert (!visited[last]);
                visited[last] = true;
                begin[last] = begin[next];
                last = next;
            }
            visited[last] = true;
            begin[last] = tmp;
        }
    }

    template <class InputIter>
    class iterator_to_function_class
    {
        InputIter _iter;
    public:
        iterator_to_function_class(InputIter iter_) : _iter(iter_){}
        
        typename ITER_UTIL::it_value_type<InputIter>::elem const & operator ()(size_t index) const{return _iter[index];};
        //typename ITER_UTIL::it_value_type<InputIter>::elem & operator ()(size_t index){return _iter[index];};
    };

    template <class InputIter>
    iterator_to_function_class<InputIter> get_iterator_to_function(InputIter iter)
    {
        return iterator_to_function_class<InputIter>(iter);
    }
    
    template <class InputIter>
    class iterator_to_function_noref_class
    {
        InputIter _iter;
    public:
        iterator_to_function_noref_class(InputIter iter_) : _iter(iter_){}
        
        typename ITER_UTIL::it_value_type<InputIter>::elem operator ()(size_t index) const{return _iter[index];};
        //typename ITER_UTIL::it_value_type<InputIter>::elem & operator ()(size_t index){return _iter[index];};
    };

    template <class InputIter>
    iterator_to_function_noref_class<InputIter> get_iterator_to_function_noref(InputIter iter)
    {
        return iterator_to_function_noref_class<InputIter>(iter);
    }

    template <class InputIter, class Func>
    class iterator_to_function_transform_class
    {
        InputIter _iter;
        Func _func;
    public:
        iterator_to_function_transform_class(InputIter iter_, Func func_) : _iter(iter_), _func(func_){}
        
        typename ITER_UTIL::it_value_type<InputIter>::elem operator ()(size_t index) const{return _func(_iter[index]);};
        //typename ITER_UTIL::it_value_type<InputIter>::elem & operator ()(size_t index){return _iter[index];};
    };

    template <class InputIter, class Func>
    iterator_to_function_transform_class<InputIter, Func> get_iterator_to_function_transform(InputIter iter, Func func)
    {
        return iterator_to_function_transform_class<InputIter, Func>(iter, func);
    }

    /*
     * sets begin[i] = begin[from_index[i]]
     * but only for a circulation
     */
    template <class InputIt, class IndexIt>
    void permutate_zirkulation_from_iter(InputIt first, IndexIt from_index)
    {
        auto tmp = *first;
        size_t last = 0;
        size_t next = 0;
        while ((next = *(from_index + next)) != 0)
        {
            *(first + last) = *(first + next);
            last = next;
        }
        *(first + last) = tmp;
    }

    /*
     * sets begin[i] = begin[from_index(i)]
     * but only for a circulation
     */    template <class InputIt, class funct>
    void permutate_zirkulation_from_function(InputIt first, funct from_index)
    {
        InputIt last = first;
        auto tmp = *last;
        InputIt next = first;
        while ((next = from_index(next)) != first)
        {
            *last = *next;
            last = next;
        }
        *last = tmp;
    }
    
    class timeout_exception : public std::exception {        
        public:
            timeout_exception() : std::exception(){}
            //timeout_exception(const char* msg) : std::exception(msg){}
            
            virtual const char* what() const throw()
            {
                return "timeout error";
            }
    };
}

/**
 * @brief Finds d such that
 * \f$ rhs * 2 ^ {d-1} < lhs <= rhs * 2 ^ d\f$
 * If lhs < rhs, then this method returns 0

 * 
 * @param lhs ...
 * @param rhs ...
 * @param current_pow ...
 * @return uint8_t
 */
template <typename T>
uint8_t get_next_pow(T lhs, T rhs, uint8_t current_pow)
{
    while (current_pow < std::numeric_limits<uint8_t>::max() && lhs > rhs * (1lu << current_pow))
    {
        ++current_pow;
    }
    while (current_pow > 0 && lhs <= rhs * (1lu << (current_pow - 1)))
    {
        --current_pow;
    }
    return current_pow;
}

/*
 * Calculates ceil(log_2(number))
 */
template <typename T>
size_t log2_upper_bound(T number)
{
    size_t erg = 0;
    while (number > 1)
    {
        number -= number / 2;
        ++erg;
    }
    return erg;
}

/*
 * Calculates floor(log_2(number))
 */
template <typename T>
size_t log2_lower_bound(T number)
{
    size_t erg = 0;
    while (number > 1)
    {
        number /= 2;
        ++erg;
    }
    return erg;
}

template<class T, class Compare>
constexpr const T& clamp( const T& v, const T& lo, const T& hi, Compare comp )
{
    return comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}
template<class T>
constexpr const T& clamp( const T& v, const T& lo, const T& hi )
{
    return clamp( v, lo, hi, std::less<T>() );
}



int64_t mulshift (int64_t a, int64_t b, int s);

#endif
