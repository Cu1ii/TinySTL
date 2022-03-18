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


}

#endif //TINYSTL_ALGO_H
