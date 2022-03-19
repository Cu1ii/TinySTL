//
// Created by Cu1 on 2022/3/18.
//

#ifndef TINYSTL_ALGO_H
#define TINYSTL_ALGO_H

// tstl 的一系列算法

#include <cstddef>
#include <ctime>

#include "algobase.h"
#include "memory.h"
#include "heap_algo.h"
#include "functional.h"

namespace tstl
{
    /*************************************************************************************/
    // all_of
    // 检查 [first, last) 内是否全部元素都满足一元操作符 unary_pred 为 true 的情况, 满足则返回 true
    /*************************************************************************************/
    template <class InputIter, class UnaryPredicate>
    bool all_of(InputIter first, InputIter last, UnaryPredicate unary_pred)
    {
        for (; first != last; ++first)
        {
            if (!unary_pred(*first))
                return false;
        }
        return true;
    }

    /*************************************************************************************/
    // any_of
    // 检查 [first, last) 内是否存在某个元素满足一元操作 unary_pred 为 true 的情况, 满足则返回 true;
    /************************************************************************************/
    template <class InputIter, class UnaryPredicate>
    bool any_of(InputIter first, InputIter last, UnaryPredicate unary_pred)
    {
        for (; first != last; ++first)
        {
            if (unary_pred(*first))
                return true;
        }
        return false;
    }

    /*************************************************************************************/
    // none_of
    // 检查 [first, last) 内是部元素都不满足一元操作 unary_pred 为 true 的情况，满足则返回 true
    /************************************************************************************/
    template <class InputIter, class UnaryPredicate>
    bool none_of(InputIter first, InputIter last, UnaryPredicate unary_pred)
    {
        for (; first != last; ++first)
        {
            if (unary_pred(*first))
                return false;
        }
        return true;
    }

    /*************************************************************************************/
    // count
    // 检查 [first, last) 区间内的元素与给定值进行比较, 缺省定义使用 operator==, 返回元素相等的个数
    /************************************************************************************/
    template <class InputIter, class T>
    size_t count(InputIter first, InputIter last, const T& value)
    {
        size_t n = 0;
        for (; first != last; ++first)
        {
            if (*first == value)
                ++n;
        }
        return n;
    }

    /*************************************************************************************/
    // count_if
    // 对 [first, last) 区间内的元素都进行一元 unary_pred 操作, 返回结果为 true 的个数
    /************************************************************************************/
    template <class InputIter, class UnaryPredicate>
    size_t count_if(InputIter first, InputIter last, UnaryPredicate unary_pred)
    {
        size_t n = 0;
        for (; first != last; ++first)
        {
            if (unary_pred(*first))
                ++n;
        }
        return n;
    }

    /*************************************************************************************/
    // find
    // 对 [first, last) 区间内找到等于 value 的元素, 返回指向该元素的迭代器
    /************************************************************************************/
    template <class InputIter, class T>
    InputIter
    find(InputIter first, InputIter last, const T& value)
    {
        while (first != last && *first != value)
            ++first;
        return first;
    }

    /*************************************************************************************/
    // find_if
    // 对 [first, last) 区间内找到第一个令一元操作 unary_pred 为 true 的元素并返回指向该元素的迭代器
    /************************************************************************************/
    template <class InputIter, class UnaryPredicate>
    InputIter
    find_if(InputIter first, InputIter last, UnaryPredicate unary_pred)
    {
        while (first != last && !unary_pred(*first))
            ++first;
        return first;
    }

    /*************************************************************************************/
    // find_if_not
    // 对 [first, last) 区间内找到第一个令一元操作 unary_pred 为 false 的元素并返回指向该元素的迭代器
    /************************************************************************************/
    template <class InputIter, class UnaryPredicate>
    InputIter
    find_if_not(InputIter first, InputIter last, UnaryPredicate unary_pred)
    {
        while (first != last && unary_pred(*first))
            ++first;
        return first;
    }

    // TODO 理解原理 136 - 208
    /*************************************************************************************/
    // search
    // [first1, last1) 中查找 [first2, last2) 的首次出现点
    /************************************************************************************/
    template <class ForwardIter1, class ForwardIter2>
    ForwardIter1
    search(ForwardIter1 first1, ForwardIter1 last1,
           ForwardIter2 first2, ForwardIter2 last2) {
        auto d1 = tstl::distance(first1, last1);
        auto d2 = tstl::distance(first2, last2);

        if (d1 < d2)
            return last1;
        auto current1 = first1;
        auto current2 = first2;
        while (current2 != last2)
        {
            if (*current1 == *current2)
            {
                ++current1;
                ++current2;
            }
            else
            {
                if (d1 == d2)
                    return last1;
                else
                {
                    current1 = ++first1;
                    current2 = ++first2;
                    --d1;
                }
            }
        }
        return first1;
    }

    // 重载版本使用 仿函数 cmp 代替比较操作
    template <class ForwardIter1, class ForwardIter2, class Compare>
    ForwardIter1
    search(ForwardIter1 first1, ForwardIter1 last1,
           ForwardIter2 first2, ForwardIter2 last2, Compare cmp)
    {
        auto d1 = tstl::distance(first1, last1);
        auto d2 = tstl::distance(first2, last2);

        if (d1 < d2)
            return last1;
        auto current1 = first1;
        auto current2 = first2;
        while (current2 != last2)
        {
            if (cmp(*current1, *current2))
            {
                ++current1;
                ++current2;
            }
            else
            {
                if (d1 == d2)
                    return last1;
                else
                {
                    current1 = ++first1;
                    current2 = ++first2;
                    --d1;
                }
            }
        }
        return first1;
    }

    /*************************************************************************************/
    // search_n
    // [first1, last1) 中查找连续 n 个 value 所形成的子序列, 返回一个迭代器指向该子序列的起始处
    /************************************************************************************/
    template <class ForwardIter, class Size, class T>
    ForwardIter
    search_n(ForwardIter first, ForwardIter last, Size n, const T& value)
    {
        if (n <= 0)
            return first;
        else
        {
            first = tstl::find(first, last, value);
            while (first != last)
            {
                auto m = n - 1;
                auto i = first;
                ++i;
                while (i != last && m != 0 && *i = value)
                    ++i,
                    --m;
                if (m == 0)
                {
                    return first;
                }
                else
                {
                    first = tstl::find(i, last, value);
                }
            }
        }
        return last;
    }

    // 重载版本使用 仿函数 cmp 代替比较操作
    template <class ForwardIter, class Size, class T, class Compare>
    ForwardIter
    search_n(ForwardIter first, ForwardIter last,
             Size n, const T& value, Compare cmp)
    {
        if (n <= 0)
            return first;
        else
        {
            while (first != last)
            {
                if (cmp(*first, value))
                    break;
                ++first;
            }
            while (first != last)
            {
                auto m = n - 1;
                auto i = first;
                ++i;
                while (i != last && m != 0 && cmp(*i, value))
                    ++i,
                    --m;
                if (m == 0)
                {
                    return first;
                }
                else
                {
                    while (i != last)
                    {
                        if (cmp(*i, value))
                            break;
                        ++i;
                    }
                    first = i;
                }
            }
        }
        return last;
    }

}

#endif //TINYSTL_ALGO_H
