#include "pair_id.h"

pair_id_injection::id_type pair_id_injection::operator ()(pair_id_injection::element_type i, pair_id_injection::element_type j) const
{
    return _pair_to_id_data[i * _num_elements + j];
}

std::array<pair_id_injection::element_type, 2> const & pair_id_injection::operator [](pair_id_injection::id_type i) const
{
    return _id_to_pair[i];
}

std::vector<pair_id_injection::id_type>::const_iterator pair_id_injection::get_row(pair_id_injection::element_type row) const
{
    return _pair_to_id_data.begin() + row * _num_elements;
}

pair_id_injection::pair_id_injection(pair_id_injection::element_type n) : pair_id_injection(n, [](pair_id_injection::element_type i, pair_id_injection::element_type j){return i != j;})
{}
