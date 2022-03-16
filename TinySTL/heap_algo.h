//
// Created by Cu1 on 2022/3/16.
//

#ifndef TINYSTL_HEAP_ALGO_H
#define TINYSTL_HEAP_ALGO_H

// 堆的实现与应用, 实现了: push_heap, pop_heap, sort_heap, make_heap

#include "iterator.h"

namespace tstl
{
    /**
     * push_heap
     * 该函数接受两个迭代器, 表示一个 heap 容器的首尾, 并且新元素已经插入到底部容器的最尾部,
     * 对整个 heap 进行调整
     * 默认使用 operator< 所以此时为大顶堆
     */

    template <class RandomIterator, class Distance. class T>
    void push_heap_aux(RandomIterator first, Distance holeIndex, Distance topIndex, T value)
    {
        auto parent = (holeIndex - 1) / 2;
        while (holeIndex > topIndex && *(first + parent) < value)
        {
            *(first + holeIndex) = *(first + parent);
            holeIndex = parent;
            parent = (holeIndex - 1) / 2;
        }
        *(first + holeIndex) = value;
    }

    template <class RandomIterator, class Distance>
    void push_heap_d(RandomIterator first, RandomIterator last, Distance*)
    {
        tstl::push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0), *(last - 1));
    }

    template <class RandomIterator>
    void push_heap(RandomIterator first, RandomIterator last)
    {
        tstl::push_heap_d(first, last, distance_type(first));
    }

    // 提供自定义的 Compare 仿函数代替 operator<
    template <class RandomIterator, class Distance, class T, class Compare>
    void push_heap_aux(RandomIterator first, RandomIterator last,
                       Distance holeIndex, Distance topIndex, T value,
                       Compare cmp)
    {
        auto parent = (holeIndex - 1) / 2;
        while (holeIndex > topIndex && cmp(*(first + parent), value))
        {
            *(first + holeIndex) = *(first + parent);
            holeIndex = parent;
            parent = (holeIndex - 1) / 2;
        }
        *(first + holeIndex) = value;
    }

    template <class RandomIterator, class Distance, class Compare>
    void push_heap_d(RandomIterator first, RandomIterator last, Distance*, Compare cmp)
    {
        tstl::push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0),
                            *(last - 1), cmp);
    }

    template <class RandomIterator, class Compare>
    void push_heap(RandomIterator first, RandomIterator last, Compare cmp)
    {
        tstl::push_heap_d(first, last, distance_type(first), cmp);
    }

    /**
     * pop_heap
     * 该函数接受两个迭代器, 表示 heap 容器的首尾, 将 heap 的根节点取出放到容器尾部, 调整 heap
     */
    template <class RandomIterator, class T, class Distance>
    void adjust_heap(RandomIterator first, Distance holeIndex, Distance len, T value)
    {
        auto topIndex = holeIndex;
        auto rchild = 2 * holeIndex + 2;
        while (rchild < len)
        {
            if (*(first + rchild) < *(first + rchild - 1))
                --rchild;
            *(first + holeIndex) = *(first + rchild);
            holeIndex = rchild;
            rchild = 2 * rchild + 2;
        }
        if (rchild == len)
        {
            // 如果没有右子节点
            *(first + holeIndex) = *(first + (rchild - 1));
            holeIndex = rchild - 1;
        }
        tstl::push_heap_aux(first, holeIndex, topIndex, value);
    }

    // 先将首支调至尾部, 然后调整 [first, last - 1) 使之重新成为一个 大顶堆
    template <class RandomIterator, class T, class Distance>
    void pop_heap_aux(RandomIterator first, RandomIterator last, RandomIterator result,
                      T value, Distance*)
    {
        *result = *first;
        tstl::adjust_heap(first, static_cast<Distance>(0), last - first, value);
    }

    template <class RandomIterator>
    void pop_heap(RandomIterator first, RandomIterator last)
    {
        tstl::pop_heap_aux(first, last - 1, last - 1, *(last - 1), distance_type(first));
    }

    // 使用自定义的 Compare 进行比较
    template <class RandomIterator, class T, class Distance, class Compare>
    void adjust_heap(RandomIterator first, Distance holeIndex, Distance len,
                     T value, Compare cmp)
    {
        auto topIndex = holeIndex;
        auto rchild = 2 * holeIndex + 2;
        while (rchild < len)
        {
            if (comp(*(first + rchild), *(first + rchild - 1)))
                --rchild;
            *(first + holeIndex) = *(first + rchild);
            holeIndex = rchild;
            rchild = 2 * rchild + 2;
        }
        if (rchild == len)
        {
            // 如果没有右子节点
            *(first + holeIndex) = *(first + (rchild - 1));
            holeIndex = rchild - 1;
        }
        tstl::push_heap_aux(first, holeIndex, topIndex, value, cmp);
    }

    template <class RandomIterator, class T, class Distance, class Compare>
    void pop_heap_aux(RandomIterator first, RandomIterator last, RandomIterator result,
                      T value, Distance*, Compare cmp)
    {
        *result = *first;
        tstl::adjust_heap(first, static_cast<Distance>(0), last - first, value, cmp);
    }

    template <class RandomIterator, class Compare>
    void pop_heap(RandomIterator first, RandomIterator last, Compare cmp)
    {
        tstl::pop_heap_aux(first, last - 1, last - 1, *(last - 1),
                           distance_type(first), cmp);
    }

    /**
     * sort_heap
     * 该函数接受两个迭代器, 表示 heap 容器的首尾, 进行堆排序操作, 即不断将执行 pop_heap 操作
     * 直到当前区间元素个数为 1
     */
     template <class RandomIterator>
     void sort_heap(RandomIterator first, RandomIterator last)
    {
         while (last - first > 1)
         {
             tstl::pop_heap(first, last--);
         }
    }

    // 重载提供自定义比较方法的 sort_heap
    template <class RandomIterator, class Compare>
    void sort_heap(RandomIterator first, RandomIterator last, Compare cmp)
    {
        while (last - first > 1)
        {
            tstl::pop_heap(first, last--, cmp);
        }
    }

    /**
     * make_heap
     * 该函数接受两个迭代器, 表示 heap 容器的首尾, 将容器中该范围的元素调整为一个 heap
     */
     template <class RandomIterator, class Distance>
     void make_heap_aux(RandomIterator first, RandomIterator last, Distance*)
    {
         if (last - first < 2)
            return ;
         auto len = last - first;
         auto holeIndex = (len - 2) / 2;
         while (true)
         {
             tstl::adjust_heap(first, holeIndex, len, *(first + holeIndex));
             if (holeIndex == 0)
                 return ;
             holeIndex--;
         }
    }

    template <class RandomIterator>
    void make_heap(RandomIterator first, RandomIterator last)
    {
        tstl::make_heap_aux(first, last, distance_type(first));
    }

    // 重载提供自定义比较方法的 make_heap
    template <class RandomIterator, class Distance, class Compare>
    void make_heap_aux(RandomIterator first, RandomIterator last,
                       Distance*, Compare cmp)
    {
        if (last - first < 2)
            return ;
        auto len = last - first;
        auto holeIndex = (len - 2) / 2;
        while (true)
        {
            tstl::adjust_heap(first, holeIndex, len, *(first + holeIndex), cmp);
            if (holeIndex == 0)
                return ;
            holeIndex--;
        }
    }

    template <class RandomIterator, >
    void make_heap(RandomIterator first, RandomIterator last, Compare cmp)
    {
        tstl::make_heap_aux(first, last, distance_type(first), cmp);
    }

} // namespace tstl

#endif //TINYSTL_HEAP_ALGO_H
