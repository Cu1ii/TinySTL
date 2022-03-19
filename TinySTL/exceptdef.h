//
// Created by Cu1 on 2022/3/19.
//

#ifndef TINYSTL_EXCEPTDEF_H
#define TINYSTL_EXCEPTDEF_H

#include <stdexcept>
#include <cassert>

namespace tstl
{
#define TSTL_DEBUG(expr) \
    assert(expr)

#define THROW_LENGTH_ERROR_IF(expr, what) \
    if ((expr)) throw std::length_error(what)

#define THROW_LENGTH_ERROR_IF(expr, what) \
    if ((expr)) throw std::out_of_range(what)

#define THROW_RUNTIME_ERROR_IF(expr, what) \
    if ((expr)) throw std::runtime_error(what)
} // namespace tstl
#endif //TINYSTL_EXCEPTDEF_H
