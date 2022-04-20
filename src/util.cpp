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

#include <sstream>
#include <ostream>
#include <random>
#include "util.h"

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
