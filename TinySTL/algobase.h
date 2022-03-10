//
// Created by Cu1 on 2022/3/10.
//

#ifndef TINYSTL_ALGOBASE_H
#define TINYSTL_ALGOBASE_H

// 实现了基本算法
#include <cstring>
#include "iterator.h"
#include "util.h"

namespace tstl
{
    // max 二者取最大 并返回最大值
    template <class T>
    const T& max(const T& lhs, const T& rhs)
    {
        return lhs < rhs ? rhs : lhs;
    }

    // max 函数使用者提供比较函数对象 true 表示 lhs < rhs
    template <class T, class Compare>
    const T& max(const T& lhs, const T& rhs, Compare cmp)
    {
        return cmp(lhs, rhs) ? rhs : lhs;
    }

    // min 二者取最小 并返回最小值
    template <class T>
    const T& min(const T& lhs, const T& rhs)
    {
        return lhs < rhs ? lhs : rhs;
    }

    // min 函数使用者提供比较函数对象 true 表示 lhs < rhs
    template <class T, class Compare>
    const T& min(const T& lhs, const T& rhs, Compare cmp)
    {
        return cmp(rhs, lhs) ? rhs : lhs;
    }

    /**
     * @name iter_swap
     * @details 将两个迭代器所指对象交换
     */
     template <class FIter1, class FIter2>
     void iter_swap(FIter1 lhs, FIter2 rhs)
    {
         tstl::swap(*lhs, *rhs);
    }

    /**
     * @name copy
     * @details 将 [first, last) 区间内元素全部拷贝到 [result, result + (last - first)) 内
     * @return 返回 result + (last - first)
     */

    // input_iterator_tag 版本
    template <class InputIterator, class OutputIterator>
    OutputIterator
    unchecked_copy_cat(InputIterator first, InputIterator last,
                       OutputIterator result ,tstl::input_iterator_tag)
    {
        for (; first != last; ++first, ++result)
        {
            *result = *first;
        }
        return result;
    }

    // random_access_iterator_tag 版本
    template <class RandomIterator, class OutputIterator>
    OutputIterator
    unchecked_copy_cat(RandomIterator first, RandomIterator last,
                       OutputIterator result, tstl::random_access_iterator_tag)
    {
        for (auto n = last - first; n > 0; --n, ++first, ++result)
        {
            *result = *first;
        }
        return result;
    }

    template <class InputIterator, class OutputIterator>
    OutputIterator
    unchecked_copy(InputIterator first, InputIterator last, OutputIterator result)
    {
        return unchecked_copy_cat(first, last, result, iterator_category(first));
    }

    // 为 trivially_copy_assignable 类型提供特化版本
    template <class Tp, class Up>
    typename std::enable_if<
            std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
            std::is_trivially_copy_assignable<Up>::value, Up*>::type
    unchecked_copy(Tp* first, Tp* last, Up* result)
    {
        const size_t n = static_cast<size_t>(last - first);
        if (n != 0)
            std::memmove(result, first, n * sizeof(Up));
        return result + n;
    }

    template <class InputIterator, class OutputIterator>
    OutputIterator
    copy(InputIterator first, InputIterator last, OutputIterator result)
    {
        return unchecked_copy(first, last, result);
    }

    /**
     * copy_backward
     * 将 [first, last) 区间内的元素复制到 [result - (last - first), result) 内
     * 返回头地址
     */

    // unchecked_copy_backward_cat 的 bidirectional_iterator_tag 版本
    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2
    unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
                                BidirectionalIter2 result, tstl::bidirectional_iterator_tag)
    {
        while (first != last)
        {
            *(--result) = *(--last);
        }
        return result;
    }

    // unchecked_copy_backward_cat 的 random_access_iterator_tag 版本
    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2
    unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
                                BidirectionalIter2 result, tstl::random_access_iterator_tag)
    {
        for (auto n = last - first; n > 0; --n)
            *(--result) = *(--last);
        return result;
    }

    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2
    unchecked_copy_backward(BidirectionalIter1 first, BidirectionalIter1 last,
                                BidirectionalIter2 result)
    {
        return unchecked_copy_backward_cat(first, last, result, iterator_category(first));
    }

    // 为 trivially_copy_assignable 类型提供特化版本
    template <class Tp, class Up>
    typename std::enable_if<
            std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
            std::is_trivially_copy_assignable<Up>::value, Up*>::type
    unchecked_copy_forward(Tp* first, Tp* last, Up* result)
    {
        const auto n = static_cast<size_t>(last - first);
        if (n != 0)
        {
            result -= n;
            std::memmove(result, first, n * sizeof(Up));
        }
        return result;
    }

    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2
    copy_forward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result)
    {
        return unchecked_copy_forward(first, last, result);
    }

    /**
     * copy_if
     * 把 [first, last) 内满足一元操作运算 unary_pred 的元素拷贝到以 result 为起始的位置上
     */
     template <class InputIterator, class OutputIterator, class UnaryPredicate>
     OutputIterator
     copy_if(InputIterator first, InputIterator last, OutputIterator result, UnaryPredicate unary_pred)
     {
        for (; first != last; ++first)
        {
            if (unary_pred(*first))
                *result++ = *first;
        }
        return result;
     }


}


#endif //TINYSTL_ALGOBASE_H
