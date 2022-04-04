//
// Created by Cu1 on 2022/4/4.
//

#ifndef TINYSTL_SET_H
#define TINYSTL_SET_H

// 该头文件包含 set / multiset

#include "rb_tree.h"
#include "functional.h"
#include "algobase.h"

namespace mystl
{

    // 模板类 set<Key, Compare> 以 rb_tree 作为底层容器, 键值不允许重复
    template <class Key, class Compare = mystl::less<Key>>
    class set
    {
    public:
        typedef Key         key_type;
        typedef Key         value_type;
        typedef Compare     key_compare;
        typedef Compare     value_compare;

    protected:
        typedef mystl::rb_tree<key_type, value_type,
                                mystl::identity<value_type>, key_compare> rep_type;
        rep_type tree;
    public:
        typedef typename rep_type::pointer                  pointer;
        typedef typename rep_type::const_pointer            const_pointer;
        typedef typename rep_type::const_reference          reference;
        typedef typename rep_type::const_reference          const_reference;
        typedef typename rep_type::const_iterator           iterator;
        typedef typename rep_type::const_iterator           const_iterator;
        typedef typename rep_type::const_reverse_iterator   reverse_iterator
        typedef typename rep_type::const_reverse_iterator   const_reverse_iterator;
        typedef typename rep_type::size_type                size_type;
        typedef typename rep_type::difference_type          difference_type;
        typedef typename rep_type::node_allocator           allocator_type;

    public:
        set() = default;

        template <class InputIter>
        set(InputIter first, InputIter last)
            : tree()
        { tree.insert_unique(first, last); }

        set(std::initializer_list<value_type>ilist)
            : tree()
        { tree.insert_unique(ilist.begin(), ilist.end()); }

        set(const set<Key>& other)
            : tree(other.tree)
        {
        }

        set(set<Key>&& other) noexcept
            : tree(mystl::move(other.tree))
        {
        }

        set<Key>& operator=(const set<Key>& rhs)
        {
            tree = rhs.tree;
            return *this;
        }

        set<Key>& operator=(set<Key>&& rhs)
        {
            tree = mystl::move(rhs.tree);
            return *this;
        }

        set<Key>& operator=(std::initializer_list<value_type> ilist)
        {
            tree.clear();
            tree.insert_unique(ilist.begin(), ilist.end());
            return *this;
        }

        key_compare     key_comp()   const { return tree.key_compare; }
        value_compare   value_comp() const { return tree.key_compare; }

        // 迭代器
        iterator                begin()            noexcept
        { return tree.begin(); }
        const_iterator          begin()      const noexcept
        { return tree.begin(); }
        iterator                end()              noexcept
        { return tree.end(); }
        const_iterator          end()        const noexcept
        { return tree.end(); }

        reverse_iterator        rbegin()           noexcept
        { return reverse_iterator(end()); }
        const_reverse_iterator  rbegin()     const noexcept
        { return const_reverse_iterator(end()); }
        reverse_iterator        rend()             noexcept
        { return reverse_iterator(begin()); }
        const_reverse_iterator  rend()       const noexcept
        { return const_reverse_iterator(begin()); }

        const_iterator          cbegin()     const noexcept
        { return begin(); }
        const_iterator          cend()       const noexcept
        { return end(); }
        const_reverse_iterator  crbegin()    const noexcept
        { return rbegin(); }
        const_reverse_iterator  crend()      const noexcept
        { return rend(); }

        bool                    empty()      const noexcept { return tree.empty(); }
        size_type               size()       const noexcept { return tree.size(); }
        size_type               max_size()   const noexcept { return tree.max_size(); }

        template <class ...Args>
        mystl::pair<iterator, bool> emplace(Args&& ...args)
        { return tree.emplace_unique(mystl::forward<Args>(args)...); }


        template <class ...Args>
        iterator emplace_hint(iterator hint, Args&& ...args)
        { return tree.emplace_unique_use_hint(hint, mystl::forward<Args>(args)...); }

        pair<iterator, bool> insert(const value_type& value)
        { return tree.insert_unique(value); }

        pair<iterator, bool> insert(value_type&& value)
        { return tree.insert_unique(mystl::move(value)); }

        iterator insert_hint(iterator hint, const value_type& value)
        { return tree.insert_unique(hint, value); }

        iterator insert_hint(iterator hint, value_type&& value)
        { return tree.insert_unique(hint, mystl::move(value)); }

        template <class InputIter>
        void insert(InputIter first, InputIter last)
        { tree.insert_unique(first, last); }

        void        erase(iterator postition)            { tree.erase(postition); }
        size_type   erase(const key_type& key)           { return tree.erase_unique(key); }
        void        erase(iterator first, iterator last) { return tree.erase(first, last); }

        void        clear() { tree.clear(); }

    public:

        iterator        find(const key_type& key)              { return tree.find(key); }
        const_iterator  find(const key_type& key)        const { return tree.find(key); }

        size_type       count(const key_type& key)       const { return tree.count_unique(key); }

        iterator        lower_bound(const key_type& key)       { return tree.lower_bound(key); }
        const_reference lower_bound(const key_type& key) const { return tree.lower_bound(key); }

        iterator        upper_bound(const key_type& key)       { return tree.upper_bound(key); }
        const_reference upper_bound(const key_type& key) const { return tree.upper_bound(key); }

        pair<iterator, iterator>
        equal_range(const key_type& key)
        { return tree.equal_range_unique(key); }

        pair<const_iterator, const_iterator>
        equal_range(const key_type& key) const
        { return tree.equal_range_unique(key); }

        void swap(set<Key>& rhs) noexcept
        { tree.swap(rhs.tree); }

    public:
        bool operator==(const set<Key>& rhs) { return tree == rhs.tree; }
        bool operator<(const set<Key>& rhs)  { return tree < rhs.tree; }

        bool operator!=(const set<Key>& rhs) { return !(*this == rhs); }
        bool operator>(const set<Key>& rhs)  { return rhs < *this; }
        bool operator<=(const set<Key>& rhs) { return !(rhs < *this); }
        bool operator>=(const set<Key>& rhs) { return !(*this < rhs); }
    };

    // 模板类 mulitset 键值允许重复
    template <class Key, class Compare = mystl::less<Key>>
    class mulitset
    {
    public:
        typedef Key         key_type;
        typedef Key         value_type;
        typedef Compare     key_compare;
        typedef Compare     value_compare;

    protected:
        typedef mystl::rb_tree<key_type, value_type,
                                mystl::identity<value_type>, key_compare> rep_type;
        rep_type tree;
    public:
        typedef typename rep_type::pointer                  pointer;
        typedef typename rep_type::const_pointer            const_pointer;
        typedef typename rep_type::const_reference          reference;
        typedef typename rep_type::const_reference          const_reference;
        typedef typename rep_type::const_iterator           iterator;
        typedef typename rep_type::const_iterator           const_iterator;
        typedef typename rep_type::const_reverse_iterator   reverse_iterator
        typedef typename rep_type::const_reverse_iterator   const_reverse_iterator;
        typedef typename rep_type::size_type                size_type;
        typedef typename rep_type::difference_type          difference_type;
        typedef typename rep_type::node_allocator           allocator_type;

    public:
        mulitset() = default;

        template <class InputIter>
        mulitset(InputIter first, InputIter last)
            : tree()
        { tree.insert_multi(first, last); }

        mulitset(std::initializer_list<value_type>ilist)
            : tree()
        { tree.insert_multi(ilist.begin(), ilist.end()); }

        mulitset(const mulitset<Key>& other)
            : tree(other.tree)
        {
        }

        mulitset(mulitset<Key>&& other) noexcept
            : tree(mystl::move(other.tree))
        {
        }

        mulitset<Key>& operator=(const mulitset<Key>& rhs)
        {
            tree = rhs.tree;
            return *this;
        }

        mulitset<Key>& operator=(mulitset<Key>&& rhs)
        {
            tree = mystl::move(rhs.tree);
            return *this;
        }

        mulitset<Key>& operator=(std::initializer_list<value_type> ilist)
        {
            tree.clear();
            tree.insert_multi(ilist.begin(), ilist.end());
            return *this;
        }

        key_compare             key_comp()   const { return tree.key_compare; }
        value_compare           value_comp() const { return tree.key_compare; }

        // 迭代器
        iterator                begin()            noexcept
        { return tree.begin(); }
        const_iterator          begin()      const noexcept
        { return tree.begin(); }
        iterator                end()              noexcept
        { return tree.end(); }
        const_iterator          end()        const noexcept
        { return tree.end(); }

        reverse_iterator        rbegin()           noexcept
        { return reverse_iterator(end()); }
        const_reverse_iterator  rbegin()     const noexcept
        { return const_reverse_iterator(end()); }
        reverse_iterator        rend()             noexcept
        { return reverse_iterator(begin()); }
        const_reverse_iterator  rend()       const noexcept
        { return const_reverse_iterator(begin()); }

        const_iterator          cbegin()     const noexcept
        { return begin(); }
        const_iterator          cend()       const noexcept
        { return end(); }
        const_reverse_iterator  crbegin()    const noexcept
        { return rbegin(); }
        const_reverse_iterator  crend()      const noexcept
        { return rend(); }

        bool                    empty()      const noexcept { return tree.empty(); }
        size_type               size()       const noexcept { return tree.size(); }
        size_type               max_size()   const noexcept { return tree.max_size(); }

        template <class ...Args>
        iterator emplace(Args&& ...args)
        { return tree.emplace_multi(mystl::forward<Args>(args)...); }


        template <class ...Args>
        iterator emplace_hint(iterator hint, Args&& ...args)
        { return tree.template emplace_multi_use_hint(hint, mystl::forward<Args>(args)...); }

        iterator insert(const value_type& value)
        { return tree.insert_multi(value); }

        iterator insert(value_type&& value)
        { return tree.insert_multi(mystl::move(value)); }

        iterator insert_hint(iterator hint, const value_type& value)
        { return tree.insert_multi(hint, value); }

        iterator insert_hint(iterator hint, value_type&& value)
        { return tree.insert_multi(hint, mystl::move(value)); }

        template <class InputIter>
        void insert(InputIter first, InputIter last)
        { tree.insert_multi(first, last); }

        void        erase(iterator postition)            { tree.erase(postition); }
        size_type   erase(const key_type& key)           { return tree.erase_multi(key); }
        void        erase(iterator first, iterator last) { return tree.erase(first, last); }

        void        clear() { tree.clear(); }

    public:

        iterator        find(const key_type& key)              { return tree.find(key); }
        const_iterator  find(const key_type& key)        const { return tree.find(key); }

        size_type       count(const key_type& key)       const { return tree.count_multi(key); }

        iterator        lower_bound(const key_type& key)       { return tree.lower_bound(key); }
        const_reference lower_bound(const key_type& key) const { return tree.lower_bound(key); }

        iterator        upper_bound(const key_type& key)       { return tree.upper_bound(key); }
        const_reference upper_bound(const key_type& key) const { return tree.upper_bound(key); }

        pair<iterator, iterator>
        equal_range(const key_type& key)
        { return tree.equal_range_multi(key); }

        pair<const_iterator, const_iterator>
        equal_range(const key_type& key) const
        { return tree.equal_range_multi(key); }

        void swap(multiset<Key>& rhs) noexcept
        { tree.swap(rhs.tree); }

    public:
        bool operator==(const multiset<Key>& rhs) { return tree == rhs.tree; }
        bool operator<(const multiset<Key>& rhs)  { return tree < rhs.tree; }

        bool operator!=(const multiset<Key>& rhs) { return !(*this == rhs); }
        bool operator>(const multiset<Key>& rhs)  { return rhs < *this; }
        bool operator<=(const multiset<Key>& rhs) { return !(rhs < *this); }
        bool operator>=(const multiset<Key>& rhs) { return !(*this < rhs); }
    };
}


#endif //TINYSTL_SET_H
