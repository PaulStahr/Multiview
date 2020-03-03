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

#ifndef ITERATOR_UTIL
#define ITERATOR_UTIL
#include <array>
namespace ITER_UTIL
{
    template <typename IT>
    struct it_value_type
    {
        typedef typename std::iterator_traits<IT>::value_type elem;
    };

    template <typename Container>
    struct it_value_type<std::back_insert_iterator<Container> >
    {
        typedef typename Container::value_type elem;
    };

    template <typename Container>
    struct it_value_type<std::front_insert_iterator<Container> >
    {
        typedef typename Container::value_type elem;
    };
        
    template <typename T>
    struct it_value_type<std::vector<T> >
    {
        typedef typename T::value_type elem;
    };
    
    template <typename T>
    struct it_value_type<std::array<T,1> >
    {
        typedef typename T::value_type elem;
    };
    
    template <typename T>
    struct it_value_type<std::array<T,2> >
    {
        typedef typename T::value_type elem;
    };
    
    template <typename T>
    struct it_value_type<std::array<T,3> >
    {
        typedef typename T::value_type elem;
    };
     
    template <class InputIter>
    class array_to_function_class
    {
        InputIter const & _iter;
    public:
        array_to_function_class(InputIter const & iter_) : _iter(iter_){}
        
        typename ITER_UTIL::it_value_type<InputIter>::elem const & operator ()(size_t index) const{return _iter[index];};
        typename ITER_UTIL::it_value_type<InputIter>::elem & operator ()(size_t index){return _iter[index];};
    };

    template <class InputIter>
    array_to_function_class<InputIter> get_array_to_function(InputIter const & iter)
    {
        return array_to_function_class<InputIter>(iter);
    }
}
#endif
