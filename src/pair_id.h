#pragma once
#include "util.h"
#include <iostream>

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


template <typename InputIter>
std::ostream & print_matrix(std::ostream & out, pair_id_injection const & pair_id, InputIter iter)
{
    size_t num_elems = pair_id._num_elements;
    for (size_t i = 0; i < num_elems; ++i)
    {
        auto row = pair_id.get_row(i);
        for (size_t j = 0; j < num_elems; ++j)
        {
            if (j != 0)
            {
                out << ' ';
            }
            if (i == j)
            {
                out << '0';
            }
            else
            {
                out << iter[row[j]];
            }
        }
        out << std::endl;
    }
    return out;
}
