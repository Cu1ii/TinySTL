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
                    erase(mystl::copy(rhs.begin(), rhs.end(), start), finish);
                else
                {
                    const_iterator mid = rhs.begin() + distance_type(len);
                    mystl::copy(rhs.begin(), mid, start);
                    insert(finish. mid, rhs.end());
                }
            }
            return *this;
        }

        deque& operator=(deque&& rhs)
        {
            clear();
            start = mystl::move(rhs.start);
            finish = mystl::move(rhs.finish);
            map_ = rhs.map_;
            map_size = rhs.map_size;
            rhs.map_ = nullptr;
            rhs.map_size = 0;
            return *this;
        }

        deque& operator=(std::initializer_list<value_type> ilist)
        {
            deque tmp(ilist);
            swap(tmp);
            return *this;
        }

        void swap(deque& other) noexcept
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

        void assign(size_type n, const value_type& value)
        {
            fill_assign(n, value);
        }

        template<class Iter, typename std::enable_if<
                mystl::is_input_iterator<Iter>::value, int>::type = 0>
        void assign(Iter first, Iter last)
        {
            copy_assign(first, last, iterator_category(first));
        }

        void assign(std::initializer_list<value_type> ilist)
        {
            copy_assign(ilist.begin(), ilist.end(), mystl::forward_iterator_tag());
        }

        // emplace_front() / emplace_back() / emplace()
        template <class ...Args>
        void emplace_front(Args&& ...args);

        template <class ...Args>
        void emplace_back(Args&& ...args);

        template <class ...Args>
        void emplace(iterator pos, Args&& ...args);

        // push_front() / push_back()
        void push_front(const value_type& value)
        {
            if (start.cur != start.first)
            {
                mystl::construct(start.cur - 1, value);
                --start.cur;
            }
            else
            {
                reserve_elements_at_front(1);
                try
                {
                    --start;
                    data_allocator::construct(start.cur, value);
                }
                catch (...)
                {
                    ++start;
                    throw ;
                }
            }
        }

        void push_back(const value_type& value)
        {
            if (finish.cur != finish.last - 1)
            {
                data_allocator::construct(finish.cur, value);
                ++finish.cur;
            }
            else
            {
                reserve_elements_at_back(1);
                data_allocator::construct(finish.cur);
                ++finish;
            }
        }

        void push_front(value_type&& value) { emplace_front(mystl::move(value)); }
        void push_back(value_type&& value) { emplace_back(mystl::move(value)); }

        // pop_back() / pop_front()

        void pop_back()
        {
            if (finish.cur != finish.first)
            {
                --finish.cur;
                data_allocator::destroy(finish.cur);
            }
            else
            {
                deallocate_node(finish.first);
                finish.set_node(finish.node - 1);
                finish.cur = finish.last - 1;
                data_allocator::destroy(finish.cur);
            }
        }

        void pop_front()
        {
            if (start.cur != start.last - 1)
            {
                data_allocator::destroy(start.cur);
                ++start.cur;
            }
            else
            {
                data_allocator::destroy(start.cur);
                deallocate_node(start.first);
                start.set_node(start.node + 1);
                start.cur = start.first;
            }
        }

        // insert()

        iterator insert(iterator postiton, const value_type& value)
        {
            if (postiton.cur == start.cur)
            {
                push_front(value);
                return start;
            }
            else if (postiton.cur == finish.cur)
            {
                push_back(value);
                iterator tmp = finish;
                --tmp;
                return tmp;
            }
            else
                return insert_aux(postiton, value);
        }

        iterator insert(iterator position)
        {
            return insert(position, value_type());
        }

        iterator insert(iterator position, value_type&& value)
        {
            emplace(position, mystl::move(value));
        }

        void insert(iterator position, size_type n, const value_type& value);

        template <class Iter, typename std::enable_if<
                mystl::is_input_iterator<Iter>::value, int>::type = 0>
        void insert(iterator position, Iter first, Iter last)
        {
            insert(pos, first, last, iterator_category(first));
        }

        void resize(size_type new_size)
        {
            resize(new_size, value_type());
        }


    public:
        // erase() / clear()

        iterator erase(iterator position)
        {
            iterator nex = pos;
            ++nex;
            auto index = pos - start;
            if (index < (size() >> 1))
            {
                mystl::copy_backward(start, position, nex);
                pop_front();
            }
            else
            {
                mystl::copy(nex, finish, pos);
                pop_back();
            }
            return start + index;
        }

        iterator erase(iterator first, iterator last);

        void clear();

    protected:
        // Internal construction/destruction
        void create_map_and_nodes(size_type num_elements);

        void destroy_map_and_nodes();

        void fill_initialize(size_type n, const value_type& value);

        template <class InputIter>
        void range_initialize(InputIter first, InputIter last, input_iterator_tag);

        template <class ForwardIter>
        void range_initialize(ForwardIter first, ForwardIter last, forward_iterator_tag);


    protected:
        // Internal assign functions
        void fill_assign(size_type n, const value_type& value);

        template <class InputIter>
        void copy_assign(InputIter first, InputIter last, input_iterator_tag);

        template <class ForwardIter>
        void copy_assign(ForwardIter first, ForwardIter last, forward_iterator_tag);

    protected:
        // Internal insert functions
        template <class InputIter>
        void insert(iterator position, InputIter first, InputIter last, input_iterator_tag);

        template <class ForwardIter>
        void insert(iterator position, ForwardIter first, ForwardIter last, forward_iterator_tag);

        template <class ...Args>
        iterator insert_aux(iterator position, Args&& ...args);

        iterator insert_aux(iterator position, const value_type& value);

        void insert_aux(iterator position, size_type n, const value_type& value);

        template <class ForwardIter>
        void insert_aux(iterator position, ForwardIter first, ForwardIter last, size_type n);

        iterator reserve_elements_at_front(size_type n)
        {
            size_type vacancies = start.cur - start.first;
            if (n > vacancies)
                new_elements_at_front(n - vacancies);
            return start - static_cast<difference_type>(n);
        }

        iterator reserve_elements_at_back(size_type n)
        {
            size_type vacancies = (finish.last - finish.cur) - 1;
            if (n > vacancies)
                new_elements_at_back(n - vacancies);
            return finish + static_cast<difference_type>(n);
        }

        void new_elements_at_front(size_type new_elements);

        void new_elements_at_back(size_type new_elements);

        void desrtoy_nodes_at_front(iterator before_start);

        void desrtoy_nodes_at_back(iterator after_finish);

    protected:
        // Allocation of map and nodes
        // 给 map 进行扩容以保证可以容纳新的节点. 实际上并没有增加节点
        // 同时扩容操作会改变 map_pointer 从而使得之前的 map_pointer 以及 deque 迭代器失效
        void resever_map_at_back(size_type nodes_to_add = 1)
        {
            if (nodes_to_add + 1 > map_size - (finish.node - map_))
                reallocate_map(nodes_to_add, false);
        }

        void resever_map_at_front(size_type nodes_to_add = 1)
        {
            if (nodes_to_add + 1 > start.node - map_)
                reallocate_map(nodes_to_add, true);
        }

        void reallocate_map(size_type nodes_to_add, bool add_at_front);

        pointer allocate_node() { return data_allocator::allocate(buffer_size); }

        void deallocate_node(pointer x) { data_allocator::deallocate(x, buffer_size); }
    };

    template <class T>
    template <class ...Args>
    void deque<T>::emplace_front(Args&& ...args)
    {
        if (start.cur != start.first)
        {
            data_allocator::construct(start.cur - 1, mystl::forward<Args>(args)...);
            --start.cur;
        }
        else
        {
            reserve_elements_at_front(1);
            try
            {
                --start;
                data_allocator::construct(start.cur, mystl::forward<Args>(args)...);
            }
            catch (...)
            {
                ++start;
                throw ;
            }
        }
    }

    template <class T>
    template <class ...Args>
    void deque<T>::emplace_back(Args&& ...args)
    {
        if (finish.cur != finish.last - 1)
        {
            data_allocator::construct(finish.cur, mystl::forward<Args>(args)...);
            ++finish.cur;
        }
        else
        {
            reserve_elements_at_back(1);
            data_allocator::construct(finish.cur, mystl::forward<Args>(args)...);
            ++finish;
        }
    }

    template <class T>
    template <class ...Args>
    typename deque<T>::iterator
    deque<T>::emplace(iterator pos, Args&& args...)
    {
        if (pos.cur == start.cur)
        {
            emplace_front(mystl::forward<Args>(args)...);
            return start;
        }
        else if (pos.cur == finish.cur)
        {
            emplace_back(mystl::forward<Args>(args)...);
            return finish - 1;
        }
        return insert_aux(pos, mystl::forward<Args>(args)...);
    }

    template <class T>
    void deque<T>::insert(iterator position, size_type n, const value_type& value)
    {
        if (position.cur == start.cur)
        {
            iterator new_start = reserve_elements_at_front(n);
            mystl::uninitialized_fill(new_start, start, value);
            start = new_start;
        }
        else if (position.cur == finish.cur)
        {
            iterator new_finish = reserve_elements_at_back(n);
            mystl::uninitialized_fill(finish, new_finish, value);
            finish = new_finish;
        }
        else
            insert_aux(position, n, value);
    }

    template <class T>
    template <class InputIter>
    void deque<T>::insert(iterator position, InputIter first, InputIter last, input_iterator_tag)
    {
        if (last <= first) return ;
        const size_type n = mystl::distance(first, last);
        const size_type elems_before = position - start;
        if (elems_before < (size() / 2))
            reserve_elements_at_front(n);
        else
            reserve_elements_at_back(n);
        position = start + elems_before;
        auto cur = --last;
        for (size_type i = 0; i < n; ++i, --cur)
            insert(position, *cur);
    }

    template <class T>
    template <class ForwardIter>
    void deque<T>::insert(iterator position, ForwardIter first, ForwardIter last, forward_iterator_tag)
    {
        if (last <= first) return ;
        const size_type n = mystl::distance(first, last);
        if (position.cur == start.cur)
        {
            reserve_elements_at_front(n);
            auto new_start = start - n;
            try
            {
                mystl::uninitialized_copy(first, last, new_start);
                start = new_start;
            }
            catch (...)
            {
                desrtoy_nodes_at_front(new_start);
                throw ;
            }
        }
        else if (position.cur == finish.cur)
        {
            reserve_elements_at_back(n);
            auto new_finish = finish + n;
            try
            {
                mystl::uninitialized_copy(first, last, finish);
                finish = new_finish;
            }
            catch (...)
            {
                desrtoy_nodes_at_back(new_finish);
                throw ;
            }
        }
        else
            insert_aux(position, first, last, n);
    }

    template <class T>
    typename deque<T>::iterator
    deque<T>::erase(iterator position)
    {
        auto nex = position;
        ++nex;
        const size_type elems_before = position - start;
        if (elems_before < (size() / 2))
        {
            mystl::copy_backward(start, position, nex);
            pop_front();
        }
        else
        {
            mystl::copy(nex, finish, position);
            pop_back();
        }
        return start + elems_before;
    }

    template <class T>
    typename deque<T>::iterator
    deque<T>::erase(iterator first, iterator last)
    {
        if (first == start && last == finish)
        {
            clear();
            return finish;
        }
        else
        {
            const size_type len = last - first;
            const size_type elems_before = first - start;
            if (elems_before < ((size() - len) / 2))
            {
                mystl::copy_backward(start, first, last);
                auto new_start = start + len;
                mystl::destroy(start, new_start);
                start = new_start;
            }
            else
            {
                mystl::copy(last, finish, first);
                auto new_finish = finish - len;
                mystl::destroy(new_finish, finish);
                finish = new_finish;
            }
            return start + elems_before;
        }
    }

    template <class T>
    void deque<T>::clear()
    {
        // clear 会保留头部缓冲区
        for (map_pointer cur = start.node + 1; cur < finish.node; ++cur)
        {
            data_allocator::destroy(*cur, *cur + buffer_size);
            data_allocator::deallocate(*cur, buffer_size);
        }
        if (start.node != finish.node)
        {
            mystl::destroy(start.cur, start.last);
            mystl::destroy(finish.first, finish.cur);
            data_allocator::deallocate(finish.first, buffer_size);
        }
        else
        {
            mystl::destroy(start.cur, finish.cur);
        }
        finish = start;
    }

    /*****************************************************************/
    // helper function
    template <class T>
    void deque<T>::create_map_and_nodes(size_type num_elements)
    {
        size_type num_nodes = num_elements / buffer_size + 1;
        map_size = max(initial_map_size, num_nodes + 2);
        map_ = map_allocator::allocate(map_size);
        map_pointer nstart = map + (map_size - num_nodes) / 2;
        map_pointer nfinish = nstart + num_nodes - 1;
        map_pointer cur = nstart;
        try
        {
            for (; cur <= nfinish; ++cur)
                *cur = allocate_node();
        }
        catch (...)
        {
            for (auto n = nstart; n < cur; ++n)
                deallocate_node(*n);
            map_allocator::deallocate(map_, map_size);
            throw ;
        }
        start.set_node(nstart);
        finish.set_node(nfinish);
        start.cur = start.first;
        finish.cur = finish.first + num_elements % buffer_size;
    }

    // 只用于 catch 子句中用作清理功能
    template <class T>
    void deque<T>::destroy_map_and_nodes()
    {
        for (auto cur = start.node; cur <= finish.node; ++cur)
            deallocate_node(*cur);
        map_allocator::deallocate(map_, map_size);
    }

    template <class T>
    void deque<T>::fill_initialize(size_type n, const value_type& value)
    {
        create_map_and_nodes(n);
        map_pointer cur;
        try
        {
            for (cur = start.node; cur < finish.node; ++cur)
                uninitialized_fill(*cur, *cur + buffer_size, value);
            uninitialized_fill(finish.first, finish.cur, value);
        }
        catch (...)
        {
            for (auto n = start.node; n < cur; ++n)
                data_allocator::destroy(*n, *n + buffer_size);
            destroy_map_and_nodes();
            throw ;
        }
    }

    template <class T>
    template <class InputIter>
    void deque<T>::range_initialize(InputIter first, InputIter last, input_iterator_tag)
    {
        create_map_and_nodes(0);
        for (; first != last; ++first)
            push_back(*first);
    }

    template <class T>
    template <class ForwardIter>
    void deque<T>::range_initialize(ForwardIter first, ForwardIter last, forward_iterator_tag)
    {
        size_type n = mystl::distance(first, last);
        create_map_and_nodes(n);
        try
        {
            mystl::uninitialized_copy(first, last, start);
        }
        catch (...)
        {
            destroy_map_and_nodes();
            throw ;
        }
    }

    template <class T>
    void deque<T>::fill_assign(size_type n, const value_type& value)
    {
        if (n > size())
        {
            mystl::fill(begin(), end(), value);
            insert(end(), n - size(), value);
        }
        else
        {
            erase(begin() + n, end());
            mystl::fill(begin(), end(), value);
        }
    }

    template <class T>
    template <class InputIter>
    void deque<T>::copy_assign(InputIter first, InputIter last, input_iterator_tag)
    {
        auto first1 = begin();
        auto last1 = end();
        for (; first != last && first1 != last1; ++first, ++first1)
            *first1 = *first1;
        if (first1 != last1)
            erase(first, last);
        else
            insert(finish, first, last, input_iterator_tag());
    }

    template <class T>
    template <class ForwardIter>
    void deque<T>::copy_assign(ForwardIter first, ForwardIter last, forward_iterator_tag)
    {
        const size_type len1 = size();
        const size_type len2 = mystl::distance(first, last);
        if (len1 < len2)
        {
            auto nex = first;
            mystl::advance(nex, len1);
            mystl::copy(first, nex, start);
            insert(finish, nex, last, forward_iterator_tag());
        }
        else
            erase(mystl::copy(first, last, start), finish);
    }

    template <class T>
    template <class... Args>
    typename deque<T>::iterator
    deque<T>::insert_aux(iterator position, Args&& ...args)
    {
        const size_type elems_before = position - start;
        value_type value_copy = value_type(mystl::forward<Args>(args));
        if (elems_before < (size() / 2))
        {
            emplace_front(front());
            auto front1 = start;
            ++front1;
            auto front2 = front1;
            ++front2;
            position = start + elems_before;
            auto pos = position;
            ++pos;
            mystl::copy(front2, pos, front1);
        }
        else
        {
            emplace_back(back());
            auto back1 = finish;
            --back1;
            auto back2 = back1;
            --back2;
            position = start + elems_before;
            mystl::copy_backward(position, back2, back1);
        }
        *position = mystl::move(value_copy);
        return position;
    }

    template <class T>
    typename deque<T>::iterator
    deque<T>::insert_aux(iterator postiton, const value_type& value)
    {
        const size_type elems_before = postiton - start;
        value_type value_copy = value;
        if (elems_before < (size() / 2))
        {
            push_front(front());
            auto front1 = start;
            ++front1;
            auto front2 = front1;
            ++front2;
            postiton = start + elems_before;
            auto pos = postiton;
            ++pos;
            mystl::copy(front2, pos, front1);
        }
        else
        {
            push_back(back());
            auto back1 = finish;
            --back1;
            auto back2 = back1;
            --back2;
            postiton = start + elems_before;
            mystl::copy_backward(postiton, back2, back1);
        }
        *postiton = value_copy;
        return postiton;
    }

    template <class T>
    void deque<T>::insert_aux(iterator position, size_type n, const value_type& value)
    {
        const size_type elems_before = position - start;
        const size_type value_copy = value;
        if (elems_before < (size() / 2))
        {
            iterator new_start = reserve_elements_at_front(n);
            iterator old_start = start;
            position = start + elems_before;
            try
            {
                if (elems_before >= n)
                {
                    iterator start_n = start + n;
                    mystl::uninitialized_copy(start, position, old_start);
                    start = new_start;
                    mystl::copy(start_n, position, old_start);
                    mystl::fill(position - n, position, value_copy);
                }
                else
                {
                    mystl::uninitialized_fill(
                            mystl::uninitialized_copy(start, position, new_start), start, value_copy
                            );
                    start = new_start;
                    mystl::fill(old_start, position, value_copy);
                }
            }
            catch (...)
            {
                destroy_map_and_nodes(new_start);
                throw ;
            }
        }
        else
        {
            iterator new_finish = reserve_elements_at_back(n);
            iterator old_finish = finish;
            const size_type elems_after = size() - elems_before;
            position = finish - elems_after;
            try
            {
                if (elems_after > n)
                {
                    auto finish_n = finish - n;
                    mystl::uninitialized_copy(finish_n, finish, finish);
                    finish = new_finish;
                    mystl::copy_backward(position, finish_n, old_finish);
                    mystl::fill(position, position + n, value_copy);
                }
                else
                {
                    mystl::uninitialized_fill(finish, position + n, value_copy);
                    mystl::uninitialized_copy(position, finish, position + n);
                    finish = new_finish;
                    mystl::fill(position, old_finish, value_copy);
                }
            }
            catch (...)
            {
                destroy_map_and_nodes(new_finish);
                throw ;
            }
        }
    }

    template <class T>
    template <class ForwardIter>
    void deque<T>::insert_aux(iterator position, ForwardIter first, ForwardIter last, size_type n)
    {
        const size_type elems_before = position - start;
        if (elems_before < (size() / 2))
        {
            auto new_start = reserve_elements_at_front(n);
            auto old_start = start;
            position = start + elems_before;
            try
            {
                if (elems_before >= n)
                {
                    auto start_n = start + n;
                    mystl::uninitialized_copy(start, start_n, new_start);
                    start = new_start;
                    mystl::copy(start_n, position, old_start);
                    mystl::copy(first, last, position - n);
                }
                else
                {
                    auto mid = first;
                    mystl::advance(mid, n - elems_before);
                    mystl::uninitialized_copy(first, mid,
                                              mystl::uninitialized_copy(start, position, new_start));
                    start = new_start;
                    mystl::copy(mid, last, old_start);
                }
            }
            catch (...)
            {
                destroy_map_and_nodes(new_start);
                throw ;
            }
        }
        else
        {
            auto new_finish = reserve_elements_at_back(n);
            auto old_finish = finish;
            const size_type elems_after = size() - elems_before;
            position = finish - elems_after;
            try
            {
                if (elems_after > n)
                {
                    auto finish_n = finish - n;
                    mystl::uninitialized_copy(finish_n, finish, finish);
                    finish = new_finish;
                    mystl::copy_backward(position, finish_n, old_finish);
                    mystl::copy(first, last, position);
                }
                else
                {
                    auto mid = first;
                    mystl::advance(mid, elems_after);
                    mystl::uninitialized_copy(position, finish,
                                              mystl::uninitialized_copy(mid, last, finish));
                    start = new_finish;
                    mystl::copy(first, mid, position);
                }
            }
            catch (...)
            {
                destroy_map_and_nodes(new_finish);
                throw ;
            }
        }
    }

    template <class T>
    void deque<T>::new_elements_at_front(size_type new_elements)
    {
        size_type new_nodes = (new_elements + buffer_size - 1) / buffer_size;
        resever_map_at_front(new_nodes);
        size_type i;
        try
        {
            for (i = 1; i <= new_nodes; ++i)
                *(finish.node - i) = allocate_node();
        }
        catch (...)
        {
            for (size_type j = 1; j < i; ++j)
                deallocate_node(*(start.node - j));
        }
    }

    template <class T>
    void deque<T>::new_elements_at_back(size_type new_elements)
    {
        size_type new_nodes = (new_elements + buffer_size - 1) / buffer_size;
        resever_map_at_back(new_nodes);
        size_type i;
        try
        {
            for (i = 1; i <= new_nodes; ++i)
                *(finish + i) = allocate_node();
        }
        catch (...)
        {
            for (size_type j = 1; j < i; ++j)
                deallocate_node(*(finish + j));
        }
    }

    template <class T>
    void deque<T>::desrtoy_nodes_at_front(iterator before_start)
    {
        for (auto n = before_start.node; n < start.node; ++n)
            deallocate_node(*n);
    }

    template <class T>
    void deque<T>::desrtoy_nodes_at_back(iterator after_finish)
    {
        for (auto n = after_finish.node; n > finish.node; --n)
            deallocate_node(*n);
    }

    template <class T>
    void deque<T>::reallocate_map(size_type nodes_to_add, bool add_at_front)
    {
        auto old_num_nodes = finish.node - start.node + 1;
        auto new_num_nodes = old_num_nodes + nodes_to_add;
        map_pointer new_nstart;
        if (map_size > 2 * new_num_nodes)
        {
            new_start = map_ + (map_size - new_num_nodes) / 2
                             + (add_at_front ? nodes_to_add : 0);
            if (new_nstart < start.node)
                mystl::copy(start.node, finish.node + 1, new_nstart);
            else
                mystl::copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
        }
        else
        {
            auto new_map_size = map_size + max(map_size, nodes_to_add) + 2;
            auto new_map = map_allocator::allocate(new_map_size);
            new_nstart = new_map + (new_map_size - new_num_nodes) / 2
                                 + (add_at_front ? nodes_to_add : 0);
            mystl::copy(start.node, finish.node + 1, new_nstart);
            map_allocator::deallocate(map_, map_size);
        }
        start.set_node(new_nstart);
        finish.set_node(new_nstart + old_num_nodes - 1);
    }

    template <class T>
    bool operator==(const deque<T>& lhs, const deque<T>& rhs)
    {
        return lhs.size() == rhs.size() && mystl::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <class T>
    bool operator!=(const deque<T>& lhs, const deque<T>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class T>
    bool opeator<(const deque<T>& lhs, const deque<T>& rhs)
    {
        return mystl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template <class T>
    bool operator>(const deque<T>& lhs, const deque<T>& rhs)
    {
        return rhs < lhs;
    }

    template <class T>
    bool operator<=(const deque<T>& lhs, const deque<T>& rhs)
    {
        return !(rhs > lhs);
    }

    template <class T>
    bool operator>=(const deque<T>& lhs, const deque<T>& rhs)
    {
        return !(lhs < rhs);
    }

    template <class T>
    void swap(deque<T>& lhs, deque<T>& rhs)
    {
        lhs.swap(rhs);
    }
}

#endif //TINYSTL_DEQUE_H
