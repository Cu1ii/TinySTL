//
// Created by Cu1 on 2022/4/17.
//

#ifndef TINYSTL_UNORDEREDSET_H
#define TINYSTL_UNORDEREDSET_H


// 该头文件包含模板类 unordered_set / unordered_multiset
// 底层容器由 hashtable 实现, 具体提供的功能与 set / multiset 类似, 容器中元素不保证有序

#include "hashtable.h"

namespace mystl
{

    template <class Value, class HashFcn = mystl::hash<Value>, class EqualKey = mystl::equal_to<Value>>
    class unordered_set
    {
    private:
        typedef hashtable<Value, Value, HashFcn, mystl::identity<Value>, EqualKey> ht;

        ht ht_;

    public:
        typedef typename ht::allocator_type allocator_type;
        typedef typename ht::key_type       key_type;
        typedef typename ht::value_type     value_type;
        typedef typename ht::hasher         hasher;
        typedef typename ht::key_equal      key_equal;

        typedef typename ht::size_type      size_type;
        typedef typename ht::difference_type   difference_type;
        typedef typename ht::pointer            pointer;
        typedef typename ht::const_pointer      const_pointer;
        typedef typename ht::reference          reference;
        typedef typename ht::const_reference    const_reference;

        typedef typename ht::const_iterator     iterator;
        typedef typename ht::const_iterator     const_iterator;

        allocator_type get_allocator() const { return ht_.get_allocator(); }

    public:

        unordered_set() : ht_(100, HashFcn(), EqualKey())
        {
        }

        explicit unordered_set(size_type bucket_count,
                               const HashFcn& hashFcn = HashFcn(),
                               const EqualKey& equalKey = EqualKey())
            : ht_(bucket_count, hashFcn, equalKey)
        {
        }

        template <class InputIter>
        unordered_set(InputIter first, InputIter last,
                      const size_type bucket_count = 100,
                      const HashFcn& hashFcn = HashFcn(),
                      const EqualKey& equalKey = EqualKey())
            : ht_(mystl::max(bucket_count, static_cast<size_type>(mystl::distance(first, last))),
                  hashFcn, equalKey)
        {
            for (; first != last; ++first)
                ht_.insert_unique_noresize(*first);
        }

        unordered_set(std::initializer_list<value_type> ilist,
                      const size_type bucket_count = 100,
                      const HashFcn& hashFcn = HashFcn(),
                      const EqualKey& equalKey = EqualKey())
            : ht_(mystl::max(bucket_count, static_cast<size_type>(ilist.size())), hashFcn, equalKey)
        {
            for (auto first = ilist.begin(); first != ilist.end(); ++first)
                ht_.insert_unique_noresize(*first);
        }

        unordered_set(const unordered_set& other)
            : ht_(other.ht_)
        {
        }

        unordered_set(unordered_set&& other) noexcept
            : ht_(mystl::move(other.ht_))
        {
        }

        unordered_set& operator=(const unordered_set& rhs)
        {
            ht_ = rhs.ht_;
            return *this;
        }

        unordered_set& operator=(unordered_set&& rhs)
        {
            ht_ = mystl::move(rhs.ht_);
            return *this;
        }

        unordered_set& operator=(std::initializer_list<value_type> ilist)
        {
            ht_.clear();
            ht_.resize(ilist.size());
            for (auto first = ilist.begin(); first != ilist.end(); ++first)
                ht_.insert_unique_noresize(*first);
            return *this;
        }

        ~unordered_set() = default;

        // iterator
        iterator        begin()       noexcept
        { return ht_.begin(); }
        const_iterator  begin() const noexcept
        { return ht_.begin(); }
        iterator        end()         noexcept
        { return ht_.end(); }
        const_iterator  end()   const noexcept
        { return ht_.end(); }

        const_iterator cbegin() const noexcept
        { return ht_.cbegin(); }
        const_iterator cend()   const noexcept
        { return ht_.cend(); }

        // 容器基本信息
        bool           empty()      const noexcept { return ht_.empty(); }
        size_type      size()       const noexcept { return ht_.size(); }
        size_type      max_size()   const noexcept { return ht_.max_size(); }

    public:

        // 容器操作
        // emplace
        template <class ...Args>
        mystl::pair<iterator, bool> emplace(Args&& ...args)
        { return ht_.emplace_unique(mystl::forward<Args>(args)...); }

        template <class ...Args>
        mystl::pair<iterator, bool> emplace_hint(const_iterator hint, Args&& ...args)
        { return ht_.emplace_unique(mystl::forward<Args>(args)...); }

        // insert
        mystl::pair<iterator, bool> insert(const value_type& value)
        { return ht_.insert_unique(value); }
        mystl::pair<iterator, bool> insert(value_type&& value)
        { return ht_.emplace_unique(mystl::move(value)); }
        mystl::pair<iterator, bool> insert(const_iterator hint, const value_type& value)
        { return ht_.insert_unique(value); }
        mystl::pair<iterator, bool> insert(const_iterator hint, value_type&& value)
        { return ht_.emplace_unique(mystl::move(value)); }

        template <class InputIter>
        void insert(InputIter first, InputIter last)
        {
            ht_.template insert_unique(first, last);
        }

        // erase / clear
        void erase(iterator it)
        { ht_.erase(it); }
        void erase(const key_type& key)
        { ht_.erase(key); }
        void erase(iterator first, iterator last)
        { ht_.erase(first, last); }
        void clear()
        { ht_.clear(); }

        void swap(unordered_set& rhs) noexcept
        { ht_.swap(rhs.ht_); }

        size_type count(const key_type& key)
        { return ht_.count(key); }

        iterator find(const key_type& key)
        { return ht_.find(key); }

        const_iterator find(const key_type& key) const
        { return ht_.find(key); }

        mystl::pair<iterator, iterator> equal_range(const key_type& key)
        { return ht_.equal_range(key); }

        mystl::pair<const_iterator, const_iterator> equal_range(const key_type& key) const
        { return ht_.equal_range(key); }

        size_type bucket_count() const noexcept
        { return ht_.bucket_count(); }

        size_type max_bucket_count() const noexcept
        { return ht_.max_bucket_count(); }

        size_type bucket_size(const key_type& key) const noexcept
        { return ht_.count(key); }

        void rehash(size_type count) { ht_.resize(count); }

        hasher hash_fcn() const { return ht_.hash_funct(); }
        key_equal key_eq() const { return ht_.key_eq(); }

        bool operator==(const unordered_set& rhs)
        {
            return this->ht_ == rhs.ht_;
        }

        bool operator!=(const unordered_set& rhs)
        {
            return !(this->ht_ == rhs.ht_);
        }
    };

    template <class Value, class HashFcn, class EqualKey>
    void swap(unordered_set<Value, HashFcn, EqualKey>& lhs,
              unordered_set<Value, HashFcn, EqualKey>& rhs)
    {
        lhs.swap(rhs);
    }


    template <class Value, class HashFcn = mystl::hash<Value>, class EqualKey = mystl::equal_to<Value>>
    class unordered_multiset
    {
    private:
        typedef hashtable<Value, Value, HashFcn, mystl::identity<Value>, EqualKey> ht;

        ht ht_;

    public:
        typedef typename ht::allocator_type allocator_type;
        typedef typename ht::key_type       key_type;
        typedef typename ht::value_type     value_type;
        typedef typename ht::hasher         hasher;
        typedef typename ht::key_equal      key_equal;

        typedef typename ht::size_type      size_type;
        typedef typename ht::difference_type   difference_type;
        typedef typename ht::pointer            pointer;
        typedef typename ht::const_pointer      const_pointer;
        typedef typename ht::reference          reference;
        typedef typename ht::const_reference    const_reference;

        typedef typename ht::const_iterator     iterator;
        typedef typename ht::const_iterator     const_iterator;

        allocator_type get_allocator() const { return ht_.get_allocator(); }

    public:

        unordered_multiset() : ht_(100, HashFcn(), EqualKey())
        {
        }

        explicit unordered_multiset(size_type bucket_count,
                               const HashFcn& hashFcn = HashFcn(),
                               const EqualKey& equalKey = EqualKey())
            : ht_(bucket_count, hashFcn, equalKey)
        {
        }

        template <class InputIter>
        unordered_multiset(InputIter first, InputIter last,
                      const size_type bucket_count = 100,
                      const HashFcn& hashFcn = HashFcn(),
                      const EqualKey& equalKey = EqualKey())
            : ht_(mystl::max(bucket_count, static_cast<size_type>(mystl::distance(first, last))),
                  hashFcn, equalKey)
        {
            for (; first != last; ++first)
                ht_.insert_multi_noresize(*first);
        }

        unordered_multiset(std::initializer_list<value_type> ilist,
                      const size_type bucket_count = 100,
                      const HashFcn& hashFcn = HashFcn(),
                      const EqualKey& equalKey = EqualKey())
            : ht_(mystl::max(bucket_count, static_cast<size_type>(ilist.size())), hashFcn, equalKey)
        {
            for (auto first = ilist.begin(); first != ilist.end(); ++first)
                ht_.insert_multi_noresize(*first);
        }

        unordered_multiset(const unordered_multiset& other)
            : ht_(other.ht_)
        {
        }

        unordered_multiset(unordered_multiset&& other) noexcept
            : ht_(mystl::move(other.ht_))
        {
        }

        unordered_multiset& operator=(const unordered_multiset& rhs)
        {
            ht_ = rhs.ht_;
            return *this;
        }

        unordered_multiset& operator=(unordered_multiset&& rhs)
        {
            ht_ = mystl::move(rhs.ht_);
            return *this;
        }

        unordered_multiset& operator=(std::initializer_list<value_type> ilist)
        {
            ht_.clear();
            ht_.resize(ilist.size());
            for (auto first = ilist.begin(); first != ilist.end(); ++first)
                ht_.insert_multi_noresize(*first);
            return *this;
        }

        ~unordered_multiset() = default;

        // iterator
        iterator        begin()       noexcept
        { return ht_.begin(); }
        const_iterator  begin() const noexcept
        { return ht_.begin(); }
        iterator        end()         noexcept
        { return ht_.end(); }
        const_iterator  end()   const noexcept
        { return ht_.end(); }

        const_iterator cbegin() const noexcept
        { return ht_.cbegin(); }
        const_iterator cend()   const noexcept
        { return ht_.cend(); }

        // 容器基本信息
        bool           empty()      const noexcept { return ht_.empty(); }
        size_type      size()       const noexcept { return ht_.size(); }
        size_type      max_size()   const noexcept { return ht_.max_size(); }

    public:

        // 容器操作
        // emplace
        template <class ...Args>
        iterator emplace(Args&& ...args)
        { return ht_.emplace_multi(mystl::forward<Args>(args)...); }

        template <class ...Args>
        iterator emplace_hint(const_iterator hint, Args&& ...args)
        { return ht_.emplace_multi(mystl::forward<Args>(args)...); }

        // insert
        iterator insert(const value_type& value)
        { return ht_.emplace_multi(value); }
        iterator insert(value_type&& value)
        { return ht_.emplace_multi(mystl::move(value)); }
        iterator insert(const_iterator hint, const value_type& value)
        { return ht_.emplace_multi(value); }
        iterator insert(const_iterator hint, value_type&& value)
        { return ht_.emplace_multi(mystl::move(value)); }

        template <class InputIter>
        void insert(InputIter first, InputIter last)
        {
            ht_.insert_multi(first, last);
        }

        // erase / clear
        void erase(iterator it)
        { ht_.erase(it); }
        void erase(const key_type& key)
        { ht_.erase(key); }
        void erase(iterator first, iterator last)
        { ht_.erase(first, last); }
        void clear()
        { ht_.clear(); }

        void swap(unordered_multiset& rhs) noexcept
        { ht_.swap(rhs.ht_); }

        size_type count(const key_type& key)
        { return ht_.count(key); }

        iterator find(const key_type& key)
        { return ht_.find(key); }

        const_iterator find(const key_type& key) const
        { return ht_.find(key); }

        mystl::pair<iterator, iterator> equal_range(const key_type& key)
        { return ht_.equal_range(key); }

        mystl::pair<const_iterator, const_iterator> equal_range(const key_type& key) const
        { return ht_.equal_range(key); }

        size_type bucket_count() const noexcept
        { return ht_.bucket_count(); }

        size_type max_bucket_count() const noexcept
        { return ht_.max_bucket_count(); }

        size_type bucket_size(const key_type& key) const noexcept
        { return ht_.count(key); }

        void rehash(size_type count) { ht_.resize(count); }

        hasher hash_fcn() const { return ht_.hash_funct(); }
        key_equal key_eq() const { return ht_.key_eq(); }

        bool operator==(const unordered_multiset& rhs)
        {
            return this->ht_ == rhs.ht_;
        }

        bool operator!=(const unordered_multiset& rhs)
        {
            return !(this->ht_ == rhs.ht_);
        }
    };

    template <class Value, class HashFcn, class EqualKey>
    void swap(unordered_multiset<Value, HashFcn, EqualKey>& lhs,
              unordered_multiset<Value, HashFcn, EqualKey>& rhs)
    {
        lhs.swap(rhs);
    }

}



#endif //TINYSTL_UNORDEREDSET_H
