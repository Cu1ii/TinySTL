//
// Created by Cu1 on 2022/4/12.
//

#ifndef TINYSTL_MAP_H
#define TINYSTL_MAP_H

// 这个头文件包含了两个模板类 map / mulitmap
// map:      映射, 元素具有键值和实值, 元素在 map 中按照键值默认升序序排列, 且键值在 map 中唯一
// multimap: 映射, 元素具有键值和实值, 元素在 multimap 中按照键值默认升序排序, 键值在 multimap 中不唯一

#include "rb_tree.h"
#include "functional.h"


namespace mystl
{
    // 模板类 map 键值唯一
    // 参数一表示键值类型, 参数二表示对应的实际值类型, 参数三表示确定键值优先级的比较方式, 默认采取 < 比较
    template <class Key, class T, class Compare = mystl::less<T>>
    class map
    {
    public:
        typedef Key                         key_type;
        typedef T                           data_type;
        typedef T                           mapped_type;
        typedef mystl::pair<const Key, T>   value_type;
        typedef Compare                     key_compare;

        // 定义仿函数, 用来进行元素的比较
        class value_compare
                : public mystl::binary_function<value_type, value_type, bool>
        {
            friend class map<Key, T, Compare>;
        private:
            Compare comp;
            value_compare(Compare c) : comp(c) {}
        public:
            bool operator()(const value_type& lhs, const value_type& rhs) const
            {
                return comp(lhs.first, rhs.first);
            }
        };

        class select1st
                : public mystl::unarg_function<value_type, T>
        {
            friend class map<Key, T, Compare>;

        public:
            T operator()(const value_type& value) const
            {
                return value.first;
            }

        };

    private:
        typedef mystl::rb_tree<key_type, value_type,
                               select1st, key_compare> rep_type;
        rep_type t;

    public:
        typedef typename rep_type::pointer                  pointer;
        typedef typename rep_type::const_pointer            const_pointer;
        typedef typename rep_type::reference                reference;
        typedef typename rep_type::const_reference          const_reference;
        typedef typename rep_type::iterator                 iterator;
        typedef typename rep_type::const_iterator           const_iterator;
        typedef typename rep_type::reverse_iterator         reverse_iterator;
        typedef typename rep_type::const_reverse_iterator   const_reverse_iterator;
        typedef typename rep_type::size_type                size_type;
        typedef typename rep_type::difference_type          difference_type;
        typedef typename rep_type::allocator_type           allocator_type;


    public:
        // 构造 / 复制 / 移动 / 重载赋值运算符
        map() = default;

        template<class InputIter>
        map(InputIter first, InputIter last)
            :t()
        { t.insert_unique(first, last); }

        map(std::initializer_list<value_type> ilist)
            :t()
        { t.insert_unique(ilist.begin(), ilist.end()); }

        explicit map(const map<Key, T, Compare>& other)
            :t(other.t)
        {
        }

        explicit map(map<Key, T, Compare>&& other)
            :t(mystl::move(other.t))
        {
        }

        map<Key, T, Compare>& operator=(const map<Key, T, Compare>& rhs)
        {
            t = rhs.t;
            return *this;
        }

        map<Key, T, Compare>& operator=(map<Key, T, Compare>&& rhs)
        {
            t = rhs.t;
            return *this;
        }

        map<Key, T, Compare>& operator=(std::initializer_list<value_type> ilist)
        {
            t.clear();
            t.insert_unique(ilist.begin(), ilist.end());
            return *this;
        }

        key_compare     key_comp()      const { return key_compare(); }
        value_compare   value_comp()    const { return value_compare(key_comp()); }
        allocator_type  get_allocator()   const { return allocator_type(); }

        // 返回迭代器
        iterator        begin()                   noexcept
        { return t.begin(); }
        const_iterator          begin()     const noexcept
        { return t.begin(); }
        iterator                end()             noexcept
        { return t.end(); }
        const_iterator          end()       const noexcept
        { return t.end(); }

        reverse_iterator        rbegin()          noexcept
        { return reverse_iterator(end()); }
        const_reverse_iterator  rbegin()    const noexcept
        { return const_reverse_iterator(end()); }
        reverse_iterator        rend()            noexcept
        { return reverse_iterator(begin()); }
        const_reverse_iterator  rend()      const noexcept
        { return const_reverse_iterator(begin()); }

        const_iterator          cbegin()    const noexcept
        { return begin(); }
        const_iterator          cend()      const noexcept
        { return end(); }
        const_reverse_iterator  crbegin()   const noexcept
        { return rbegin(); }
        const_reverse_iterator  crend()     const noexcept
        { return rend(); }


        bool                    empty()     const noexcept { return t.empty(); }
        size_type               size()      const noexcept { return t.size(); }
        size_type               max_size()  const noexcept { return t.max_size(); }

        // 访问内部元素
        mapped_type& at(const key_type& key)
        {
            iterator it = lower_bound(key);

            // 不存在元素则抛出异常
            THROW_OUT_OF_RANGE_IF(it == end() || key_comp()(it->first, key),
                                  "map<Key, T> no such element exists");
            return it->second;
        }

        const mapped_type& at(const key_type& key) const
        {
            iterator it = lower_bound(key);

            // 不存在元素则抛出异常
            THROW_OUT_OF_RANGE_IF(it == end() || key_comp()(it->first, key),
                                  "map<Key, T> no such element exists");
            return it->second;
        }

        mapped_type& operator[](const key_type& key)
        {
            iterator it = lower_bound(key);
            if (it == end() || key_comp()(key, it->first))
                it = emplace_hint(it, key, T());
            return it->second;
        }

        mapped_type& operator[](key_type&& key)
        {
            iterator it = lower_bound(key);
            if (it == end() || key_comp()(key, it->first))
                it = emplace_hint(it, mystl::move(key), T());
            return it->second;
        }

        // 插入删除相关

        template <class ...Args>
        mystl::pair<iterator, bool> emplace(Args&& ...args)
        {
            return t.emplace_unique(mystl::forward<Args>(args)...);
        }

        template <class ...Args>
        iterator emplace_hint(iterator hint, Args&& ...args)
        {
            return t.emplace_unique_use_hint(hint, mystl::forward<Args>(args)...);
        }

        mystl::pair<iterator, bool> insert(const value_type& value)
        {
            return t.insert_unique(value);
        }

        mystl::pair<iterator, bool> insert(value_type&& value)
        {
            return t.insert_unique(mystl::move(value));
        }

        iterator insert(iterator hint, const value_type& value)
        {
            return t.insert_unique(hint, value);
        }

        iterator insert(iterator hint, value_type&& value)
        {
            return t.insert_unique(hint, mystl::move(value));
        }

        template <class InputIter>
        void insert(InputIter first, InputIter last)
        {
            t.insert_unique(first, last);
        }

        void        erase(iterator position)             { t.erase(position); }
        size_type   erase(const key_type& key)           { return t.erase_unique(key); }
        void        erase(iterator first, iterator last) { t.erase(first, last); }

        void        clear()                              { t.clear(); }

        iterator        find(const key_type& key)               { return t.find(key); }
        const_iterator  find(const key_type& key)        const  { return t.find(key); }

        size_type       count(const key_type& key)       const  { return t.count_unique(key); }

        iterator        lower_bound(const key_type& key)        { return t.lower_bound(key); }
        const_iterator  lower_bound(const key_type& key) const  { return t.lower_bound(key); }

        iterator        upper_bound(const key_type& key)        { return t.upper_bound(key); }
        const_iterator  upper_bound(const key_type& key) const  { return t.upper_bound(key); }

        mystl::pair<iterator, iterator>
            equal_range(const key_type& key)
        { return t.equal_range_unique(key); }

        mystl::pair<const iterator, const iterator>
            equal_range(const key_type& key) const
        { return t.equal_range_unique(key); }

        void swap(map<Key, T, Compare>& rhs) noexcept
        { t.swap(rhs.t); }

    public:
        // 运算符重载
        bool operator==(const map<Key, T, Compare>& rhs) { return this->t == rhs.t; }
        bool operator<(const map<Key, T, Compare>& rhs)  { return this->t < rhs.t; }
        bool operator!=(const map<Key, T, Compare>& rhs) { return !(this->t == rhs.t); }
        bool operator>(const map<Key, T, Compare>& rhs)  { return rhs.t < this->t; }
        bool operator<=(const map<Key, T, Compare>& rhs) { return !(rhs.t < this->t); }
        bool operator>=(const map<Key, T, Compare>& rhs) { return !(this->t < rhs.t); }
    };

    template <class Key, class T, class Compare>
    void swap(map<Key, T, Compare>& lhs, map<Key, T, Compare>& rhs)
    {
        lhs.swap(rhs);
    }

    /************************************************************************************/


    // 模板类 multimap, 键值允许重复
    // 参数一表示键值类型, 参数二表示对应的实际值类型, 参数三表示确定键值优先级的比较方式, 默认采取 < 比较
    template <class Key, class T, class Compare = mystl::less<T>>
    class multimap
    {
    public:
        typedef Key                         key_type;
        typedef T                           data_type;
        typedef T                           mapped_type;
        typedef mystl::pair<const Key, T>   value_type;
        typedef Compare                     key_compare;

        // 定义仿函数, 用来进行元素的比较
        class value_compare
                : public mystl::binary_function<value_type, value_type, bool>
        {
            friend class map<Key, T, Compare>;
        private:
            Compare comp;
            value_compare(Compare c) : comp(c) {}
        public:
            bool operator()(const value_type& lhs, const value_type& rhs) const
            {
                return comp(lhs.first, rhs.first);
            }
        };

        class select1st
                : public mystl::unarg_function<value_type, T>
        {
            friend class map<Key, T, Compare>;

        public:
            T operator()(const value_type& value) const
            {
                return value.first;
            }

        };

    private:
        typedef mystl::rb_tree<key_type, value_type,
                               select1st, key_compare> rep_type;
        rep_type t;

    public:
        typedef typename rep_type::pointer                  pointer;
        typedef typename rep_type::const_pointer            const_pointer;
        typedef typename rep_type::reference                reference;
        typedef typename rep_type::const_reference          const_reference;
        typedef typename rep_type::iterator                 iterator;
        typedef typename rep_type::const_iterator           const_iterator;
        typedef typename rep_type::reverse_iterator         reverse_iterator;
        typedef typename rep_type::const_reverse_iterator   const_reverse_iterator;
        typedef typename rep_type::size_type                size_type;
        typedef typename rep_type::difference_type          difference_type;
        typedef typename rep_type::allocator_type           allocator_type;


    public:
        // 构造 / 复制 / 移动 / 重载赋值运算符
        multimap() = default;

        template<class InputIter>
        multimap(InputIter first, InputIter last)
            :t()
        { t.insert_multi(first, last); }

        multimap(std::initializer_list<value_type> ilist)
            :t()
        { t.insert_multi(ilist.begin(), ilist.end()); }

        explicit multimap(const multimap<Key, T, Compare>& other)
            :t(other.t)
        {
        }

        explicit multimap(multimap<Key, T, Compare>&& other)
            :t(mystl::move(other.t))
        {
        }

        multimap<Key, T, Compare>& operator=(const multimap<Key, T, Compare>& rhs)
        {
            t = rhs.t;
            return *this;
        }

        multimap<Key, T, Compare>& operator=(multimap<Key, T, Compare>&& rhs)
        {
            t = rhs.t;
            return *this;
        }

        multimap<Key, T, Compare>& operator=(std::initializer_list<value_type> ilist)
        {
            t.clear();
            t.insert_multi(ilist.begin(), ilist.end());
            return *this;
        }

        key_compare     key_comp()      const { return key_compare(); }
        value_compare   value_comp()    const { return value_compare(key_comp()); }
        allocator_type  get_allocator()   const { return allocator_type(); }

        // 返回迭代器
        iterator        begin()                   noexcept
        { return t.begin(); }
        const_iterator          begin()     const noexcept
        { return t.begin(); }
        iterator                end()             noexcept
        { return t.end(); }
        const_iterator          end()       const noexcept
        { return t.end(); }

        reverse_iterator        rbegin()          noexcept
        { return reverse_iterator(end()); }
        const_reverse_iterator  rbegin()    const noexcept
        { return const_reverse_iterator(end()); }
        reverse_iterator        rend()            noexcept
        { return reverse_iterator(begin()); }
        const_reverse_iterator  rend()      const noexcept
        { return const_reverse_iterator(begin()); }

        const_iterator          cbegin()    const noexcept
        { return begin(); }
        const_iterator          cend()      const noexcept
        { return end(); }
        const_reverse_iterator  crbegin()   const noexcept
        { return rbegin(); }
        const_reverse_iterator  crend()     const noexcept
        { return rend(); }


        bool                    empty()     const noexcept { return t.empty(); }
        size_type               size()      const noexcept { return t.size(); }
        size_type               max_size()  const noexcept { return t.max_size(); }


        // 插入删除相关

        template <class ...Args>
        iterator emplace(Args&& ...args)
        {
            return t.emplace_multi(mystl::forward<Args>(args)...);
        }

        template <class ...Args>
        iterator emplace_hint(iterator hint, Args&& ...args)
        {
            return t.emplace_multi_use_hint(hint, mystl::forward<Args>(args)...);
        }

        iterator insert(const value_type& value)
        {
            return t.insert_multi(value);
        }

        iterator insert(value_type&& value)
        {
            return t.insert_multi(mystl::move(value));
        }

        iterator insert(iterator hint, const value_type& value)
        {
            return t.insert_multi(hint, value);
        }

        iterator insert(iterator hint, value_type&& value)
        {
            return t.insert_multi(hint, mystl::move(value));
        }

        template <class InputIter>
        void insert(InputIter first, InputIter last)
        {
            t.insert_multi(first, last);
        }

        void        erase(iterator position)             { t.erase(position); }
        size_type   erase(const key_type& key)           { return t.erase_multi(key); }
        void        erase(iterator first, iterator last) { t.erase(first, last); }

        void        clear()                              { t.clear(); }

        iterator        find(const key_type& key)               { return t.find(key); }
        const_iterator  find(const key_type& key)        const  { return t.find(key); }

        size_type       count(const key_type& key)       const  { return t.count_unique(key); }

        iterator        lower_bound(const key_type& key)        { return t.lower_bound(key); }
        const_iterator  lower_bound(const key_type& key) const  { return t.lower_bound(key); }

        iterator        upper_bound(const key_type& key)        { return t.upper_bound(key); }
        const_iterator  upper_bound(const key_type& key) const  { return t.upper_bound(key); }

        mystl::pair<iterator, iterator>
            equal_range(const key_type& key)
        { return t.equal_range_multi(key); }

        mystl::pair<const iterator, const iterator>
            equal_range(const key_type& key) const
        { return t.equal_range_multi(key); }

        void swap(multimap<Key, T, Compare>& rhs) noexcept
        { t.swap(rhs.t); }

    public:
        // 运算符重载
        bool operator==(const multimap<Key, T, Compare>& rhs) { return this->t == rhs.t; }
        bool operator<(const multimap<Key, T, Compare>& rhs)  { return this->t < rhs.t; }
        bool operator!=(const multimap<Key, T, Compare>& rhs) { return !(this->t == rhs.t); }
        bool operator>(const multimap<Key, T, Compare>& rhs)  { return rhs.t < this->t; }
        bool operator<=(const multimap<Key, T, Compare>& rhs) { return !(rhs.t < this->t); }
        bool operator>=(const multimap<Key, T, Compare>& rhs) { return !(this->t < rhs.t); }
    };

    template <class Key, class T, class Compare>
    void swap(multimap<Key, T, Compare>& lhs, multimap<Key, T, Compare>& rhs)
    {
        lhs.swap(rhs);
    }
}

#endif //TINYSTL_MAP_H
