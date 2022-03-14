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

     /**
      *  copy_n
      *  把 [first, first + n) 区间上的元素都拷贝到 [result, result + n) 上
      *  返回一个 pair 分别指向拷贝结束的尾部
      */
      template <class InputIterator, class Size, class OutputIterator>
      tstl::pair<InputIterator, OutputIterator>
      unchecked_copy_n(InputIterator first, Size n, OutputIterator result,
                       tstl::input_iterator_tag)
     {
        for (; n; --n, ++first, ++result)
        {
            *result = *first;
        }
        return tstl::pair<InputIterator, OutputIterator>(first, result);
     }

     template <class RandomIterator, class Size, class OutputIteratror>
     tstl::pair<RandomIterator, OutputIteratror>
     unchecked_copy_n(InputIterator first, Size n, OutputIterator result,
                      random_access_iterator_tag)
     {
         auto last = first + n;
         return tstl::pair<RandomIterator, OutputIteratror>(last, tstl::copy(first, last, result));
     }

     template <class InputIterator, class Size, class OutputIterator>
     tstl::pair<InputIterator, OutputIterator>
     copy_n(InputIterator first, Size n, OutputIterator result)
     {
         return unchecked_copy(first, n, result, iterator_category(first));
     }

     /**
      * move
      * 把 [first, last) 区间内的元素移动到 [result, result + (last - first)) 内
      */
      // input_iterator 版本
      template <class InputIterator, class OutputIterator>
      OutputIterator
      unchecked_move_cat(InputIterator first, InputIterator last, OutputIterator result,
                         input_iterator_tag)
      {
          for (; first != last; ++first, ++result)
          {
              *result = tstl::move(*first);
          }
          return result;
      }

      // ramdom_access_iterator_tag 版本
      template <class InputerIterator, class OutputIterator>
      OutputIterator
      unchecked_move_cat(InputerIterator first, InputerIterator last, OutputIterator result,
                         random_access_iterator_tag)
      {
          for (auto n = last - first; n > 0; --n, ++first, ++result)
          {
              *result = tstl::move(*first);
          }
          return result;
      }


      template <class InputIterator, class OutputIterator>
      OutputIterator
      unchecked_move(InputIterator first, InputIterator last, OutputIterator result)
      {
          return unchecked_move_cat(first, last, iterator_category(first));
      }

    // 为 trivially_copy_assignable 类型提供特化版本
    template <class Tp, class Up>
    typename std::enable_if<
            std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
            std::is_trivially_move_assignable<Up>::value,
            Up*>::type
    unchecked_move(Tp* first, Tp* last, Up* result)
    {
        const size_t n = static_cast<size_t>(last - first);
        if (n)
            std::memmove(result, first, n * sizeof Up);
        return result + n;
    }

    template <class InputIterator, class OutputIterator>
    OutputIterator move(InputIterator first, InputIterator last, OutputIterator result)
    {
        return unchecked_move(first, last, result);
    }

    /**
     * move_backward
     * 将 [first, last) 区间内的元素移动到 [result - (last - first), result) 内
     */

    // bidirectional_iterator_tag 版本
    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2
    unchecked_move_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
                                BidirectionalIter2 result, bidirectional_iterator_tag)
    {
        while (first != last)
            *(--result) = tstl::move(*(--last));
        return result;
    }

    // random_access_iterator_tag 版本
    template <class RandomIterator1, class RandomIterator2>
    RandomIterator2
    unchecked_move_backward_cat(RandomIterator1 first, RandomIterator1 last,
                                RandomIterator2 result, random_access_iterator_tag)
    {
        for (auto n = last - first; n; --n)
            *(--result) = tstl::move(*(--last));
        return result;
    }

    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2
    unchecked_move_backward(BidirectionalIter1 first, BidirectionalIter1 last,
                            BidirectionalIter2 resutl)
    {
        return unchecked_copy_backward_cat(first, last, resutl, iterator_category(first));
    }

    // 为 trivially_copy_assignable 类型提供特化版本
    template <class Tp, class Up>
    typename std::enable_if<
            std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
            std::is_trivially_move_assignable<Up>::value,
            Up*>::type
    unchecked_move_backward(Tp* first, Tp*  last, Up* result)
    {
        const size_t n = static_cast<size_t>(last - first);
        if (n)
        {
            result -= n;
            std::memmove(first, result, n * sizeof Up);
        }
    }

    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2
    move_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result)
    {
        return unchecked_copy_backward(first, last, result);
    }

    /**
     * equal
     * 比较序列 1 在 [first, last) 区间上的元素是否与序列 2 相同
     */
     template <class InputIterator1, class InputIterator2>
     bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2)
    {
         for (; first1 != last1; ++first1, ++first2)
         {
             if (*first1 != *first2)
                 return false;
         }
         return true;
    }

    // 提供自定义比较仿函数对象
    template <class InputIterator1, class InputIterator2, class Compared>
    bool  equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, Compared cmp)
    {
        for (; first1 != last1; ++first1, ++first2)
        {
            if (!cmp(*first1, *first2))
                return false;
        }
        return true;
    }

    /**
     * fill_n
     * 从 first 开始填充 n 个值
     */
     template <class OutputIterator, class Size, class T>
     OutputIterator
     unchecked_fill_n(OutputIterator first, Size n, const T& value)
    {
         for (; n; --n, ++first)
         {
             *first = value;
         }
         return first;
    }

    // 为 one-byte 类型提供特化版本
    template <class Tp, class Size, class Up>
    typename std::enable_if<
            std::is_integral<Tp>::value && sizeof(Tp) == 1 &&
            !std::is_same<Tp, bool>::value &&
            std::is_integral<Up>::value && sizeof(Up) == 1,
            Tp*>::type
    unchecked_fill_n(Tp* first, Size n, Up value)
    {
        if (n > 0)
        {
            std::memset(first, (unsigned char)value, size_t(n));
        }
        return first + n;
    }

    template <class OutputIterator, class Size, class T>
    OutputIterator
    fill_n(OutputIterator first, Size n, const T& value)
    {
        return unchecked_fill_n(first, n, value);
    }

    /**
     * fill
     * 将 [first, last) 区间内的所有元素重新填充值
     */
     template <class ForwardIterator, class T>
     void fill_cat(ForwardIterator first, ForwardIterator last, const T& value,
                   tstl::forward_iterator_tag)
    {
         for (; first != last; ++first)
         {
             *first = value;
         }
    }

    template <class RandomIterator, class T>
    void fill_cat(RandomIterator first, RandomIterator last, const T& value,
                  random_access_iterator_tag)
    {
        fill_n(first, last - first, value);
    }

    template <class ForwardIterator, class T>
    void fill(ForwardIterator first, ForwardIterator last, const T& value)
    {
        fill_cat(first, last, value, iterator_category(first));
    }

    /**
     * lexicographical_compare
     * 以字典序排序对两个序列进行比较, 当在某个位置发现第一组不相等元素是, 有下列几种情况:
     * (1) 如果第一个序列的元素较小, 就返回 true, 否则返回 false
     * (2) 如果到达 last1 时没有到达 last2, 则返回 true;
     * (3) 如果到达 last2 时没有到达 last1, 则返回 false;
     * (4) 如果同时到达 last1 和 last2 , 则返回 false
     */
     template <class InputIterator1, class InputIterator2>
     bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
                                  InputIterator2 first2, InputIterator2 last2)
    {
         for (; first2 != last2 && first1 != last1; ++first1, ++first2)
         {
             if (*first1 < *first2)
                 return true;
             if (*first2 < *first1)
                 return false;
         }
         return first1 == last1 && first2 != last2;
    }

    // 使用自定义 Compare 仿函数对象代替比较操作
    template <class InputIterator1, class InputIterator2, class Compared>
    bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
                                 InputIterator2 first2, InputIterator2 last2, Compared cmp)
    {
        for (; first2 != last2 && first1 != last1; ++first1, ++first2)
        {
            if (cmp(*first1, *first2))
                return true;
            if (cmp(*first2, *first1))
                return false;
        }
        return first1 == last1 && first2 != last2;
    }

    // 针对 const unsigned char* 的特化版本
    bool  lexicographical_compare(const unsigned char* first1,
                                  const unsigned char* last1,
                                  const unsigned char* first2,
                                  const unsigned char* last2)
    {
        const size_t len1 = last1 - first1;
        const size_t len2 = last2 - first2;
        // 先对相同部分进行比较
        const size_t result = std::memcmp(first1, first2, tstl::min(len1, len2));
        // 如果相同, 就比较谁的长度更大
        return result != 0 ? (result < 0) : (len1 < len2);
    }

    /**
     * mismatch
     * 平行比较两个序列, 找到第一处失陪的元素, 返回一对迭代器, 分别指向两个序列中失陪的元素
     */
     template <class InputIterator1, class InputIterator2>
     tstl::pair<InputIterator1, InputIterator2>
     mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2)
    {
         for (; first1 != last1 && *first1 == *first2;)
         {
             ++first1,
             ++first2;
         }
         return tstl::pair<InputIterator1, InputIterator2>(first1, first2);
    }

    // 提供自定义的 Compare 仿函数对象 cmp 代替比较操作
    template <class InputIterator1, class InputIterator2, class Compare>
    tstl::pair<InputIterator1, InputIterator2>
    mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, Compare cmp)
    {
        for (; first1 != last1 && *first1 == *first2;)
        {
            ++first1,
            ++first2;
        }
        return tstl::pair<InputIterator1, InputIterator2>(first1, first2);
    }
}


#endif //TINYSTL_ALGOBASE_H
