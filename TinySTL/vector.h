//
// Created by Cu1 on 2022/3/24.
//

#ifndef TINYSTL_VECTOR_H
#define TINYSTL_VECTOR_H

#include <initializer_list>
#include "memory.h"
#include "iterator.h"
#include "util.h"
#include "exceptdef.h"

namespace mystl
{

    // 模板类 vector
    // 模板参数 T 为数据类型
    template <class T>
    class vector
    {
        static_assert(!std::is_same<bool, T>::value, "vector<bool> is abandoned in mystl");
    public:
        typedef mystl::allocator<T>                      allocator_type;
        typedef mystl::allocator<T>                      data_allocator;

        typedef typename allocator_type::value_type      value_type;
        typedef typename allocator_type::pointer         pointer;
        typedef typename allocator_type::const_pointer   const_pointer;
        typedef typename allocator_type::reference       reference;
        typedef typename allocator_type::const_reference const_reference;
        typedef typename allocator_type::size_type       size_type;
        typedef typename allocator_type::difference_type difference_type;

        typedef value_type*                              iterator;
        typedef const value_type*                        const_iterator;
        typedef mystl::reverse_iterator<iterator>        reverse_iterator;
        typedef mystl::reverse_iterator<const_iterator>  const_reverse_iterator;

    protected:


        iterator start;
        iterator finish;
        iterator end_of_storage;

        void insert_aux(iterator position, const T& value);

        template<class ...Args>
        void insert_aux(iterator position, Args&& ...args);
        void deallocate();
        void fill_initialize(size_type n, const T& value);
        void try_init() noexcept;
        void space_initialize(size_type size, size_type cap);
        template <class Iter>
        void range_initialize(Iter first, Iter last);

    public:
        // 构造, 赋值, 析构函数
        vector()
        { try_init(); }

        explicit vector(size_type n)
        { fill_initialize(n, T()); }

        vector(size_type n, const T& value)
        { fill_initialize(n, value); }

        template <class Iter, typename std::enable_if<
                mystl::is_input_iterator<Iter>::value, int>::type = 0>
        vector(Iter first, Iter last)
        { range_initialize(first, last); }

        vector(const vector<T>& other)
        { range_initialize(other.start, other.finish); }

        vector(vector<T>&& other)
            :start(other.start),
             finish(other.finish),
             end_of_storage(other.end_of_storage)
        {
            other.start = nullptr;
            other.finish = nullptr;
            other.end_of_storage = nullptr;
        }

        vector(std::initializer_list<T> ilist)
        { range_initialize(ilist.begin(), ilist.end()); }

        vector<T>& operator=(const vector<T>& rhs);
        vector<T>& operator=(vector<T>&& rhs);

        vector<T>& operator=(std::initializer_list<T> ilist)
        {
            auto t = vector<T>(ilist.begin(), ilist.end());
            swap(t);
            return *this;
        }

        ~vector()
        {
            data_allocator::destroy(start, finish);
            deallocate();
            start = finish = end_of_storage = nullptr;
        }

    public:

        // 迭代器相关
        iterator                begin()           noexcept
        { return start; }
        const_iterator          begin()     const noexcept
        { return start; }
        iterator                end()             noexcept
        { return finish; }
        const_iterator          end()       const noexcept
        { return finish; }

        reverse_iterator        rbegin()          noexcept
        { return reverse_iterator(end()); }
        const_reverse_iterator  rbegin()    const noexcept
        { return reverse_iterator(end()); }
        reverse_iterator        rend()            noexcept
        { return const_reverse_iterator(begin());}
        const_reverse_iterator  rend()      const noexcept
        { return const_reverse_iterator(begin());}

        const_iterator          cbegin()    const noexcept
        { return begin(); }
        const_iterator          cend()      const noexcept
        { return end(); }
        const_reverse_iterator  crbegin()   const noexcept
        { return rbegin(); }
        const_reverse_iterator  crend()     const noexcept
        { return rend(); }

        // 容器基本属性
        bool      empty()         const noexcept
        { return start == finish; }
        size_type size()     const noexcept
        { return static_cast<size_type>(finish - start); }
        size_type max_size() const noexcept
        { return static_cast<size_type>(-1) / sizeof(T); }
        size_type capacity() const noexcept
        { return static_cast<size_type>(end_of_storage - start); }
        void      reserve(size_type n);

        // 访问元素相关操作
        reference operator[](size_type n)
        {
            MYSTL_DEBUG(n < size());
            return *(start + n);
        }

        const_reference operator[](size_type n) const
        {
            MYSTL_DEBUG(n < size());
            return *(start + n);
        }

        reference at(size_type n)
        {
            THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
            return *(start + n);
        }

        const_reference at(size_type n) const
        {
            THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
            return *(start + n);
        }

        reference front()
        {
            MYSTL_DEBUG(!empty());
            return *start;
        }

        const_reference front() const
        {
            MYSTL_DEBUG(!empty());
            return *start;
        }

        reference back()
        {
            MYSTL_DEBUG(!empty());
            return *(finish - 1);
        }

        const_reference back() const
        {
            MYSTL_DEBUG(!empty());
            return *(finish - 1);
        }

        // 修改容器相关操作
        // assign

        void      fill_assign(size_type n, const T& value);
        template <class IIter>
        void      copy_assign(IIter first, IIter last, input_iterator_tag);
        template <class FIter>
        void      copy_assign(FIter first, FIter last, forward_iterator_tag);
        
        void assign(size_type n, const T& value)
        { fill_assign(n, value); }

        template <class Iter, typename std::enable_if<
                mystl::is_input_iterator<Iter>::value, int>::type = 0>
        void assign(Iter first, Iter last)
        {
            MYSTL_DEBUG(!(last < first));
            copy_assign(first, last, iterator_category(first));
        }

        void assign(std::initializer_list<value_type> il)
        { copy_assign(il.begin(), il.end(), mystl::forward_iterator_tag{}); }

        // emplace, emplace_back

        template <class... Args>
        iterator emplace(const_iterator pos, Args&& ...args);

        template <class... Args>
        void emplace_back(Args&& ...args);

        void push_back(const T& value)
        {
            if (finish != end_of_storage)
            {
                data_allocator::construct(finish, value);
                ++finish;
            }
            else
                insert_aux(end(), value);
        }

        void push_back(T&& value)
        { emplace_back(forward<T>(value)); }

        void pop_back()
        {
            --finish;
            destroy(finish);
        }

        // insert

        template<class Iter>
        void range_insert(const_iterator position, Iter first, Iter last);

        iterator insert(const_iterator pos, const T& value)
        {
            auto n = pos - begin();
            if (finish != end_of_storage && pos == end())
            {
                data_allocator::construct(finish, value);
            }
            else
                insert_aux(pos, value);
            return begin() + n;
        }

        iterator insert(const_iterator pos, T&& value)
        { return emplace(pos, mystl::move(value)); }

        iterator insert(const_iterator pos)
        { return insert(pos, T());}

        void insert(const_iterator pos, size_type n, const T& value);

        template <class Iter, typename std::enable_if<
                mystl::is_input_iterator<Iter>::value, int>::type = 0>
        void insert(const_iterator pos, Iter first, Iter last)
        {
            MYSTL_DEBUG(pos >= begin() && pos <= end() && !(last < first));
            range_insert(pos, first, last);
        }

        //erase
        iterator erase(const_iterator pos);
        iterator erase(const_iterator first, const_iterator last);

    public:
        // helper functional
        void swap(vector<T>& other) noexcept;
        void clear();
        //TODO 增加 mystl::reverse
        void reverse() { reverse(begin(), end()); }
        void resize(size_type new_size) { return resize(new_size, T()); }
        void resize(size_type new_size, const T& value);
    };

    /******************************************************************************/

    template <class T>
    void vector<T>::deallocate()
    {
        if (start)
            data_allocator::deallocate(start, end_of_storage - start);
    }

    template <class T>
    void vector<T>::try_init() noexcept
    {
        try
        {
            start = data_allocator::allocate(16);
            start = finish;
            end_of_storage = start + 16;
        }
        catch (...)
        {
            start = nullptr;
            finish = nullptr;
            end_of_storage = nullptr;
        }
    }

    template <class T>
    void vector<T>::space_initialize(size_type size, size_type cap)
    {
        try
        {
            start = data_allocator::allocate(cap);
            finish = start + size;
            end_of_storage = start + cap;
        }
        catch (...)
        {
            start = nullptr;
            finish = nullptr;
            end_of_storage = nullptr;
            throw ;
        }
    }

    template <class T>
    void vector<T>::fill_initialize(size_type n, const T& value)
    {
        const size_type init_size = mystl::max(static_cast<size_type>(16), n);
        space_initialize(n, init_size);
        mystl::uninitialized_fill_n(start, n, value);
    }

    template <class T>
    template <class Iter>
    void vector<T>::range_initialize(Iter first, Iter last)
    {
        /* 此处只考虑线性容器的迭代器 */
        const size_type init_size = mystl::max(static_cast<size_type>(16),
                                               static_cast<size_type>(last - first));
        space_initialize(static_cast<size_type>(last - first), init_size);
        mystl::uninitialized_fill_n(first, last, start);
    }

    template <class T>
    vector<T>&
    vector<T>::operator=(const vector<T>& rhs)
    {
        if (this != &rhs)
        {
            const size_type len = rhs.size();
            if (len > capacity())
            {
                vector<T>t(rhs.begin(), rhs.end());
                swap(t);
            }
            else if (size() >= len)
            {
                auto i = mystl::copy((iterator)rhs.begin(), (iterator)rhs.end(), begin());
                data_allocator::destroy(i, end());
                finish = start + len;
            }
            else
            {
                mystl::copy(rhs.begin(), rhs.begin() + size(), begin());
                mystl::uninitialized_copy(rhs.begin() + size(), rhs.end(), end());
                end_of_storage = finish = start + len;
            }
            return *this;
        }

    }

    template <class T>
    vector<T>& vector<T>::operator=(vector<T>&& rhs)
    {
        clear();
        start = rhs.start;
        finish = rhs.finish;
        end_of_storage = rhs.end_of_storage;
        rhs.start = rhs.finish = rhs.end_of_storage = nullptr;
        return *this;
    }

    // 当原容量小于要求大小时才会重新分配
    template <class T>
    void vector<T>::reserve(size_type n)
    {
        if (capacity() < n)
        {
            THROW_LENGTH_ERROR_IF(n > max_size(),
                                  "n can not larger than max_size() in vector<T>::reserve(n)");
            const auto old_size = size();
            auto tmp = data_allocator::allocate(n);
            mystl::move((iterator)start, (iterator)finish, (iterator)tmp);
            clear();
            start = tmp;
            finish = tmp + old_size;
            end_of_storage = tmp + n;

        }
    }

    template <class T>
    void vector<T>::fill_assign(size_type n, const T& value)
    {
        if (n > capacity())
        {
            vector<T>tmp(n, value);
            swap(tmp);
        }
        else if (n > size())
        {
            mystl::fill(begin(), end(), value);
            finish = mystl::uninitialized_fill_n(end(), n - size(), value);
        }
        else
            erase(mystl::fill_n(begin(), n, value), end());
    }

    template <class T>
    template <class IIter>
    void vector<T>::
    copy_assign(IIter first, IIter last, input_iterator_tag)
    {
        iterator cur = start;
        for (; first != last && cur != finish; ++first, ++cur)
        {
            *cur = *first;
        }
        if (first == last)
            earse(cur, finish);
        else
            insert(finish, first, last);
    }

    template <class T>
    template <class FIter>
    void vector<T>::
    copy_assign(FIter first, FIter last, forward_iterator_tag)
    {
        auto len = mystl::distance(first, last);
        if (len > capacity())
        {
            vector tmp(first, last);
            swap(tmp);
        }
        else if (size() >= len)
        {
            iterator new_finish = mystl::copy(first, last, start);
            data_allocator::destroy(new_finish, finish);
            finish = new_finish;
        }
        else
        {
            auto mid = first;
            mystl::advance(mid, size());
            mystl::copy(first, mid, start);
            iterator new_finish = mystl::uninitialized_copy(mid, last, finish);
            finish = new_finish;
        }
    }

    // 在 pos 位置处就地构造元素, 避免额外的复制或者移动开销
    template <class T>
    template <class ...Args>
    typename vector<T>::iterator
    vector<T>::emplace(const_iterator pos, Args&& ...args)
    {
        MYSTL_DEBUG(pos >= begin() && pos <= end());
        iterator xpos = const_cast<iterator>(pos);
        const size_type n = xpos - start;
        if (finish != end_of_storage && xpos == end())
        {
            data_allocator::construct(end(), mystl::forward<Args>(args)...);
            ++finish;
        }
        else
            insert_aux(xpos, mystl::forward<Args>(args)...);
        return begin() + n;
    }

    template <class T>
    template <class ...Args>
    void vector<T>::emplace_back(Args&& ...args)
    {
        if (finish < end_of_storage)
        {
            data_allocator::construct(end(), forward<Args>(args)...);
            ++finish;
        }
        else
            insert_aux(end(), forward<Args>(args)...);
    }

    // insert

    template <class T>
    template <class ...Args>
    void vector<T>::insert_aux(iterator position, Args&& ...args)
    {
        if (finish != end_of_storage)
        {
            data_allocator::construct(finish, *(finish - 1));
            ++finish;
            copy_backward(position, finish - 2, finish - 1);
            *position = value_type(mystl::forward<Args>(args)...);
        }
        else
        {
            auto old_size = size();
            auto len = (old_size != 0 ? 2 * old_size : 1);
            iterator new_start = data_allocator::allocate(len);
            iterator new_finish = new_start;
            try
            {
                new_finish = mystl::uninitialized_copy(start, position, new_start);
                data_allocator::construct(new_finish,  mystl::forward<Args>(args)...);
                ++new_finish;
                new_finish = mystl::uninitialized_copy(position, finish, new_finish);
            }
            catch (...)
            {
                data_allocator::destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, len);
                throw ;
            }
            clear();
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + len;
        }
    }

    template <class T>
    void vector<T>::insert_aux(iterator position, const T& value)
    {
        if (finish != end_of_storage)
        {
            data_allocator::construct(finish, *(finish - 1));
            ++finish;
            auto value_copy = value;
            copy_backward(position, finish - 2, finish - 1);
            *position = value_copy;
        }
        else
        {
            auto old_size = size();
            auto len = (old_size != 0 ? 2 * old_size : 1);
            iterator new_start = data_allocator::allocate(len);
            iterator new_finish = new_start;
            try
            {
                new_finish = mystl::uninitialized_copy(start, position, new_start);
                data_allocator::construct(new_finish, value);
                ++new_finish;
                new_finish = mystl::uninitialized_copy(position, finish, new_finish);
            }
            catch (...)
            {
                data_allocator::destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, len);
                throw ;
            }
            clear();
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + len;
        }
    }


    template <class T>
    void vector<T>::insert(const_iterator pos, size_type n, const T& value)
    {
        if (n != 0)
        {
            if (static_cast<size_type>(end_of_storage - finish) >= n)
            {
                T value_copy = value;
                auto elems_after = finish - pos;
                iterator old_finish = finish;
                if (elems_after > n)
                {
                    mystl::uninitialized_copy(finish - n, finish, finish);
                    finish += n;
                    mystl::copy_backward((iterator)pos, (iterator)(old_finish - n), iterator(old_finish));
                    mystl::fill(pos, pos + n, value_copy);
                }
                else
                {
                    mystl::uninitialized_fill_n(finish, n - elems_after, value_copy);
                    finish += n - elems_after;
                    mystl::uninitialized_copy((iterator)pos, (iterator)old_finish, (iterator)finish);
                    finish += elems_after;
                    mystl::fill((iterator)pos, (iterator)old_finish, value_copy);
                }
            }
            else
            {
                auto old_size = size();
                auto len = old_size + mystl::max(old_size, n);
                iterator new_start = data_allocator::allocate(len);
                iterator new_finish = new_start;
                try
                {
                    new_finish = mystl::uninitialized_copy((iterator)start, (iterator)pos, (iterator)new_start);
                    new_finish = mystl::uninitialized_fill_n(new_finish, n, value);
                    new_finish = mystl::uninitialized_copy((iterator)pos, (iterator)finish, (iterator)new_finish);
                }
                catch (...)
                {
                    data_allocator::destroy(new_start, new_finish);
                    data_allocator::deallocate(new_start, len);
                    throw ;
                }
                clear();
                start = new_start;
                finish = new_finish;
                end_of_storage = new_start + len;
            }
        }
    }

    template <class T>
    template <class Iter>
    void vector<T>::range_insert(const_iterator position, Iter first, Iter last)
    {
        if (first != last)
        {
            size_type n = 0;
            n = mystl::distance((iterator)first, (iterator)last);
            if (static_cast<size_type>(end_of_storage - finish) >= n)
            {
                auto elems_after = finish - position;
                iterator old_finish = finish;
                if (elems_after > n)
                {
                    mystl::uninitialized_copy(finish - n, finish, finish);
                    finish += n;
                    copy_backward((iterator)position, (iterator)old_finish - n, (iterator)old_finish);
                    copy(first, last, position);
                }
                else
                {
                    auto mid = first;
                    mystl::advance(mid, elems_after);
                    mystl::uninitialized_copy(mid, last, finish);
                    finish += n - elems_after;
                    mystl::uninitialized_copy((iterator)position, (iterator)old_finish, (iterator)finish);
                    finish += elems_after;
                    mystl::copy(first, mid, position);
                }
            }
            else
            {
                const size_type old_size = size();
                const size_type len = old_size + max(old_size, n);
                iterator new_start = data_allocator::allocate(len);
                iterator new_finish = new_start;
                try
                {
                    new_finish = mystl::uninitialized_copy((iterator)start, (iterator)position, (iterator)new_start);
                    new_finish = mystl::uninitialized_copy((iterator)first, (iterator)last, (iterator)new_finish);
                    new_finish = mystl::uninitialized_copy((iterator)position, (iterator)finish, (iterator)new_finish);
                }
                catch (...)
                {
                    data_allocator::destroy(new_start, new_finish);
                    data_allocator::destroy(new_start, new_start + len);
                    throw ;
                }
                clear();
                start = new_start;
                finish = new_finish;
                end_of_storage = new_start + len;
            }
        }
    }

    template <class T>
    typename vector<T>::iterator
    vector<T>::
    erase(const_iterator pos)
    {
        MYSTL_DEBUG(pos >= begin() && pos < end());
        iterator xpos = start + (pos - start);
        mystl::move(xpos + 1, finish, xpos);
        data_allocator::destroy(finish - 1);
        --finish;
        return xpos;
    }

    template <class T>
    typename vector<T>::iterator
    vector<T>::
    erase(const_iterator first, const_iterator last)
    {
        MYSTL_DEBUG(first >= begin() && last <= end() && !(last < first));
        const auto n = first - start;
        iterator r = start + n;
        data_allocator::destroy(mystl::move(r + (last - first), finish, r), finish);
        finish = finish - (last - first);
        return begin() + n;
    }

    template <class T>
    void vector<T>::resize(size_type new_size, const T& value)
    {
        if (new_size < size())
            erase(begin() + new_size, end());
        else
            insert(end(), new_size - size(), value);
    }

    template <class T>
    void vector<T>::swap(vector<T>& other) noexcept
    {
        if (this != &other)
        {
            mystl::swap(start, other.start);
            mystl::swap(finish, other.finish);
            mystl::swap(end_of_storage, other.end_of_storage);
        }
    }

    template <class T>
    void vector<T>::clear()
    {
        data_allocator::destroy(begin(), end());
        deallocate();
        start = finish = end_of_storage = nullptr;
    }

} // namespace mystl

#endif //TINYSTL_VECTOR_H
