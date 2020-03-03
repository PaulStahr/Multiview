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