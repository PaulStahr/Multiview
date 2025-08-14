#ifndef MESH_UTILS_H
#define MESH_UTILS_H

#include "data.h"

namespace objl
{
void compress(Mesh & m);
void compress(mesh_object_t & m, bool uniform);
void compress_impl(Mesh & m, matharray<float, 3> const & min_vec, matharray<float, 3> const & max_vec);
}
#endif
