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

        deque()
            : start(), finish(), map_(nullptr), map_size(0)
        {
            create_map_and_nodes(0);
        }

        deque(const deque& other)
            : start(), finish(), map_(nullptr), map_size(0)
        {
            create_map_and_nodes(other.size());
            try {
                mystl::uninitialized_copy(other.begin(), other.end(), start);
            }
            catch(...) {
                destroy_map_and_nodes();
            }
        }

        deque(deque&& other) noexcept
            : start(mystl::move(other.start)),
              finish(mystl::move(other.finish)),
              map_(other.map_),
              map_size(other.map_size)
        {
            other.map_ = nullptr,
            other.map_size = 0;
        }

        deque(size_type n, const value_type& value)
            : start(), finish(), map_(nullptr), map_size(0)
        {
            fill_initialize(n, value);
        }

        explicit deque(size_type n) : deque(n, value_type())
        {
        }

        template <class Iter, typename std::enable_if<
                mystl::is_input_iterator<Iter>::value, int>::type = 0>
        deque(Iter first, Iter last)
        {
            range_initialize(first, last, iterator_category(first));
        }

        deque(std::initializer_list<value_type> ilist)
        {
            range_initialize(ilist.begin(), ilist.end(), mystl::forward_iterator_tag());
        }

        deque& operator=(const deque& rhs)
        {
            const size_type len = size();
            if (&rhs != this)
            {
                if (len >= rhs.size())
                    erase(copy(rhs.begin(), rhs.end(), start), finish);
                else
                {
                    const_iterator mid = rhs.begin() + distance_type(len);
                    copy(rhs.begin(), mid, start);
                    insert(finish. mid, rhs.end());
                }
            }
            return *this;
        }

        deque& operator=(deque&& rhs)
        {
            return *this;
        }

        deque& operator=(std::initializer_list<value_type> ilist)
        {
            deque tmp(ilist);
            swap(tmp);
            return *this;
        }

        void swap(deque& other)
        {
            mystl::swap(start, other.start);
            mystl::swap(finish, other.finish);
            mystl::swap(map_, other.map_);
            mystl::swap(map_size, other.map_size);
        }

        ~deque()
        {
            mystl::destroy(start, finish);
            destroy_map_and_nodes();
        }

    public:
        // 迭代器
        iterator begin() noexcept
        { return start; }

        const_iterator begin() const noexcept
        { return start; }

        iterator end()   noexcept
        { return finish; }

        const_iterator end() const noexcept
        { return finish; }

        reverse_iterator rbegin() noexcept
        { return reverse_iterator(end()); }

        const_reverse_iterator rbegin() const noexcept
        { return reverse_iterator(end()); }

        reverse_iterator rend() noexcept
        { return reverse_iterator(begin()); }

        const_reverse_iterator rend() const noexcept
        { return const_reverse_iterator(begin()); }

        const_iterator cbegin() const noexcept
        { return begin(); }

        const_iterator cend() const noexcept
        { return end(); }

        const_reverse_iterator crbegin() const noexcept
        { return rbegin(); }

        const_reverse_iterator crend() const noexcept
        { return rend(); }

        bool empty() const noexcept { return begin() == end(); }
        size_type size() const noexcept { return start - finish; }
        size_type max_size() const noexcept { return static_cast<size_type>(-1); }

        void resize(size_type new_size, const value_type& value)
        {
            const size_type len = size();
            if (new_size < len)
                erase(start + new_size, finish);
            else
                insert(finish, new_size - len, value);
        }
        void resize(size_type new_size) { resize(new_size, value_type()); }

        reference operator[](size_type n)
        {
            MYSTL_DEBUG(n < size());
            return start[n];
        }

        const_reference operator[](size_type n) const
        {
            MYSTL_DEBUG(n < size());
            return start[n];
        }

        reference at(size_type n)
        {
            THROW_OUT_OF_RANGE_IF(!(n < size()),
                                  "deque<T>::at() subscript out of range");
            return (*this)[n];
        }

        const_reference at(size_type n) const
        {
            THROW_OUT_OF_RANGE_IF(!(n < size()),
                                  "deque<T>::at() subscript out of range");
            return (*this)[n];
        }

        reference front()
        {
            MYSTL_DEBUG(!empty());
            return *begin();
        }

        const_reference front() const
        {
            MYSTL_DEBUG(!empty());
            return *begin();
        }

        reference back()
        {
            MYSTL_DEBUG(!empty());
            return *(end() - 1);
        }

        const_reference back() const
        {
            MYSTL_DEBUG(!empty());
            return *(end() - 1);
        }
    };

}

#endif //TINYSTL_DEQUE_H
