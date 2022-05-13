//
// Created by Cu1 on 2022/5/12.
//

#ifndef TINYSTL_DEQUE_H
#define TINYSTL_DEQUE_H

// 该头文件包含了一个模板类 deque
// deque: 双向队列

#include <initializer_list>
#include "iterator.h"
#include "memory.h"
#include "util.h"
#include "exceptdef.h"

namespace mystl
{

#ifdef max
#pragma message("#undefing marco max")
#undef max
#endif // max

#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif // min

    // 根据 deque 中元素的大小来分配一个分区的元素个数
    template <class T>
    struct deque_buf_size
    {
        static constexpr size_t value =
                sizeof (T) < 256 ? size_t(4096 / sizeof (T)) : 16;
    };

    template <class T, class Ref, class Ptr>
    struct deque_iterator
    {
        typedef random_access_iterator_tag              iterator_category;
        typedef deque_iterator<T, T&, T*>               iterator;
        typedef deque_iterator<T, const T&, const T*>   const_iterator;
        typedef deque_iterator                          self;

        typedef T           value_type;
        typedef Ptr         pointer;
        typedef Ref         reference;
        typedef size_t      size_type;
        typedef ptrdiff_t   difference_type;
        typedef T*          value_pointer;
        typedef T**         map_pointer;

        static const size_type buffer_size = deque_buf_size<T>::value;
        T* cur;             // 指向所在缓冲区的当前元素
        T* first;           // 指向所在缓冲区的头部
        T* last;            // 指向所在缓冲区的尾部
        map_pointer node;   // 缓冲区所在节点

        deque_iterator() noexcept
            : cur(nullptr), first(nullptr), last(nullptr), node(nullptr)
        {
        }
        deque_iterator(T* x, map_pointer y)
            : cur(x), first(*y), last(*y + buffer_size)
        {
        }
        deque_iterator(const iterator& other)
            : cur(other.cur), first(other.first), last(other.last), node(other.node)
        {
        }
        deque_iterator(iterator&& other)
            : cur(other.cur), first(other.first), last(other.last), node(other.node)
        {
            other.cur = nullptr,
            other.first = nullptr,
            other.last = nullptr,
            other.node = nullptr;
        }
        deque_iterator(const const_iterator& other)
            : cur(other.cur), first(other.first), last(other.last), node(other.node)
        {
        }

        self& operator=(const iterator& rhs)
        {
            if (this != &rhs)
            {
                cur = rhs.cur,
                first = rhs.first,
                last = rhs.last,
                node = rhs.node;
            }
            return *this;
        }

        reference operator*() const { return *cur; }
        pointer   operator->() const { return cur; }

        difference_type operator-(const self& x) const
        {
            return static_cast<difference_type>(buffer_size) * (node - x.node)
                + (cur - first) - (x.cur - x.first);
        }

        self& operator++()
        {
            ++cur;
            if (cur == last)
            {
                set_node(node + 1);
                cur = first;
            }
            return *this;
        }

        self operator++(int)
        {
            self tmp = *this;
            ++(*this);
            return tmp;
        }

        self& operator--()
        {
            if (cur == first)
            {
                set_node(node - 1);
                cur = last;
            }
            --cur;
            return *this;
        }
        self operator--(int)
        {
            self tmp = *this;
            --(*this);
            return tmp;
        }

        self& operator+=(difference_type n)
        {
            auto offset = n + (cur - first);
            if (offset >= 0 && offset < difference_type(buffer_size))
                cur += n;
            else
            {
                auto node_offset =
                        offset > 0 ? offset / static_cast<difference_type>(buffer_size)
                                    : -static_cast<difference_type>((-offset - 1) / buffer_size)  - 1;
                set_node(node + node_offset);
                cur = first + (offset - node_offset * static_cast<difference_type>(buffer_size));
            }
            return *this;
        }

        self operator+(difference_type n) const
        {
            self tmp = *this;
            return tmp += n;
        }

        self& operator-=(difference_type n) { return *this += -n; }

        self operator-(difference_type n) const
        {
            self tmp = *this;
            return tmp -= n;
        }

        void set_node(map_pointer new_node)
        {
            node = new_node;
            first = *new_node;
            last = first + buffer_size;
        }

        reference operator[](difference_type n) const { return *(*this + n); }

        bool operator==(const self& rhs) const { return cur == rhs.cur; }
        bool operator!=(const self& rhs) const { return !(*this == rhs); }
        bool operator<(const self& rhs) const
        { return (node == rhs.node) ? (cur < rhs.cur) : (node < rhs.node); }
        bool operator>(const self& rhs) { return rhs < *this; }
        bool operator<=(const self& rhs) const { return !(rhs < *this); }
        bool operator>=(const self& rhs) const { return !(*this < rhs); }
    };

    // 模板类 deque
    // 模板参数表示数据类型
    template <class T>
    class deque
    {
    public:
        typedef mystl::allocator<T>     allocator_type;
        typedef mystl::allocator<T>     data_allocator;
        typedef mystl::allocator<T>     map_allocator;

        typedef typename allocator_type::value_type         value_type;
        typedef typename allocator_type::pointer            pointer;
        typedef typename allocator_type::const_pointer      const_pointer;
        typedef typename allocator_type::reference          reference;
        typedef typename allocator_type::const_reference    const_reference;
        typedef typename allocator_type::size_type          size_type;
        typedef typename allocator_type::difference_type    difference_type;
        typedef pointer*                                    map_pointer;
        typedef const_pointer*                              const_map_pointer;

        typedef deque_iterator<T, T&, T*>                   iterator;
        typedef deque_iterator<T, const T&, const T*>       const_iterator;
        typedef mystl::reverse_iterator<iterator>           reverse_iterator;
        typedef mystl::reverse_iterator<const_iterator>     const_reverse_iterator;

        allocator_type get_allocator() { return allocator_type(); }

        static const size_type buffer_size = deque_buf_size<T>::value;
        static const size_type init_map_size = 8;

    private:
        iterator start;
        iterator finish;
        map_pointer map_;
        size_type map_size;

    };

}

#endif //TINYSTL_DEQUE_H
