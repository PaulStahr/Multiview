#include <sstream>
#include <ostream>
#include <random>
#include "util.h"

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


namespace UTIL
{
    bool endsWith(const std::string& str, const std::string& suffix)
    {
        return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
    }

    bool startsWith(const std::string& str, const std::string& prefix)
    {
        return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
    }
    
    void rand_mark(std::vector<bool> & take, size_t num, size_t seed)
    {
        std::random_device rd;
        std::mt19937 gen(seed);
        std::uniform_int_distribution<size_t> dis(0, std::numeric_limits<size_t>::max()); 
        size_t *indices = new size_t[take.size()];
        std::iota(indices, indices + take.size(), 0);
        for (size_t i = 0; i < num; i++) 
        {
            size_t & index = indices[dis(gen) % (take.size() - i)];
            take[index] = true;
            index = indices[take.size() - i - 1];
        }
    }

    insert_right_operator_struct<uint8_t, shift_right_struct> shift_right(uint8_t comp){return get_insert_right_operator(comp, shift_right_struct());}
}

int NullBuffer::overflow(int c)
{
    return c;
}

NullStream::NullStream() : std::ostream(&m_sb){}

int64_t mulshift (int64_t a, int64_t b, int s)
{
    int64_t res;
    __asm__ (
        "movq  %1, %%rax;\n\t"          // rax = a
        "movl  %3, %%ecx;\n\t"          // ecx = s
        "imulq %2;\n\t"                 // rdx:rax = a * b
        "shrdq %%cl, %%rdx, %%rax;\n\t" // rax = int64_t (rdx:rax >> s)
        "movq  %%rax, %0;\n\t"          // res = rax
        : "=rm" (res)
        : "rm"(a), "rm"(b), "rm"(s)
        : "%rax", "%rdx", "%ecx");
    return res;
}

uint64_t mulshift (uint64_t a, uint64_t b)
{
    int s = 64;
    uint64_t res;
    __asm__ (
        "movq  %1, %%rax;\n\t"          // rax = a
        "movl  %3, %%ecx;\n\t"          // ecx = s
        "imulq %2;\n\t"                 // rdx:rax = a * b
        "shrdq %%cl, %%rdx, %%rax;\n\t" // rax = int64_t (rdx:rax >> s)
        "movq  %%rax, %0;\n\t"          // res = rax
        : "=rm" (res)
        : "rm"(a), "rm"(b), "rm"(s)
        : "%rax", "%rdx", "%ecx");
    return res;
}
