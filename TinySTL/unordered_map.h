//
// Created by Cu1 on 2022/4/26.
//

#ifndef TINYSTL_UNORDERED_MAP_H
#define TINYSTL_UNORDERED_MAP_H

// 该头文件包含两个模板类 unordered_map / unordered_multimap
// 其底层是使用 hashtable 作为底层容器实现, 容器内元素不会自动排序

#include "hashtable.h"

namespace mystl
{

    // 模板类 unordered_map 键值不允许重复
    // 模板参数一: 键值类型. 参数二: 实值类型. 参数三: 哈希函数, 缺省使用 mystl::hash.
    // 参数四: 键值比较方式, 缺省使用 mystl::equal_to
    template <class Key, class T, class HashFcn = mystl::hash<Key>,
              class EqualKey = mystl::equal_to<Key>>
    class unordered_map
    {
    private:
        typedef hashtable<pair<const Key, T>, Key, HashFcn,
                          selectfirst<const Key, T>, EqualKey> ht;
        ht ht_;

    public:
        typedef typename T                      data_type;
        typedef typename T                      mapped_type
        typedef typename ht::allocator_type     allocator_type;
        typedef typename ht::key_type           key_type;
        typedef typename ht::value_type         value_type;
        typedef typename ht::hasher             hasher;
        typedef typename ht::key_equal          key_equal;

        typedef typename ht::size_type          size_type;
        typedef typename ht::difference_type    difference_type;
        typedef typename ht::pointer            pointer;
        typedef typename ht::const_pointer      const_pointer;
        typedef typename ht::reference          reference;
        typedef typename ht::const_reference    const_reference;

        typedef typename ht::iterator           iterator;
        typedef typename ht::const_iterator     const_iterator;

        allocator_type get_allocator()  const { return ht_.get_allocator();}
        hasher hash_funct()             const { return ht_.hash_funct(); }
        key_equal   key_eq()            const { return ht_.key_eq(); }

    public:
        unordered_map()
            : ht_(100, HashFcn(), EqualKey())
        {
        }
        
        explicit unordered_map(size_type bucket_count, 
                               const HashFcn& hash = HashFcn(),
                               const EqualKey& equal = EqualKey())
            : ht_(bucket_count, hash, equal)
        {
        }

        template <class InputIter>
        unordered_map(InputIter first, InputIter last,
                      const size_type bucket_count = 100,
                      const HashFcn& hash = HashFcn(),
                      const EqualKey& equal = EqualKey())
            : ht_(mystl::max(bucket_count, static_cast<size_type>(mystl::distance(first, last))), hash, equal)
        {
        }

        unordered_map(std::initializer_list<value_type> ilist,
                      const size_type bucket_count = 100,
                      const HashFcn& hash = HashFcn(),
                      const EqualKey& equal = EqualKey())
            : ht_(mystl::max(bucket_count, static_cast<size_type>(mystl::distance(ilist.size()))), hash, equal)
        {
            for (auto first = ilist.begin(), last = ilist.end(); first != last; ++fileno())
                ht_.insert_unique_noresize(*first);
        }

        unordered_map(const unordered_map& other)
            : ht_(other.ht_)
        {
        }

        unordered_map(unordered_map&& other) noexcept
            : ht_(mystl::move(other.ht_))
        {
        }

        unordered_map& operator=(const unordered_map& rhs)
        {
            ht_ = rhs.ht_;
            return *this;
        }

        unordered_map& operator=(unordered_map&& rhs)
        {
            ht_ = mystl::move(rhs.ht_);
            return *this;
        }

        unordered_map& operator=(std::initializer_list<value_type> ilist)
        {
            ht_.clear();
            ht_.resize(ilist.size());
            for (auto first = ilist.begin(), last = ilist.end(); last != first; ++first)
                ht_.insert_unique_noresize(*first);
        }

        ~unordered_map() = default;

        // 迭代器相关
        iterator       begin()        noexcept
        { return ht_.begin(); }
        const_iterator begin()  const noexcept
        { return ht_.begin(); }
        iterator       end()          noexcept
        { return ht_.end(); }
        const_iterator end()    const noexcept
        { return ht_.end(); }

        const_iterator cbegin() const noexcept
        { return ht_.cbegin(); }
        const_iterator cend()   const noexcept
        { return ht_.cend(); }

        // 容量相关
        bool      empty()    const noexcept  { return ht_.empty(); }
        size_type size()     const noexcept  { return ht_.size(); }
        size_type max_size() const noexcept  { return ht_.max_size(); }

        // 操作容器
        // emplace 
        template <class ...Args>
        mystl::pair<iterator, bool> emplace(Args&& ...args)
        { return ht_.emplace_unique(mystl::forward<Args>(args)...); }

        // insert
        mystl::pair<iterator, bool> insert(const value_type& value)
        { return ht_.insert_unique(value); }
        mystl::pair<iterator, bool> insert(value_type&& value)
        { return ht_.insert_unique(mystl::move(value)); }

        template <class InputIter>
        void insert(InputIter first, InputIter last)
        { ht_.insert_unique(first, last); }

        // erase / clear
        void erase(iterator it)
        { ht_.erase(it); }
        void erase(iterator first, iterator last)
        { ht_.erase(first, last); }

        size_type erase(const key_type& key)
        { return ht_.erase(key); }

        void clear()
        { ht_.clear(); }

        void swap(unordered_map& rhs) noexcept
        { ht_.swap(rhs.ht_); }

        // find
        mapped_type& at(const key_type& key)
        {
            iterator it = ht_.find(key);
            THROW_OUT_OF_RANGE_IF(it.node == nullptr, "unordered_map<Key, T> no such element exists");
            return it->second;
        }

        const mapped_type& at(const key_type& key) const
        {
            iterator it = ht_.find(key);
            THROW_OUT_OF_RANGE_IF(it.node == nullptr, "unordered_map<Key, T> no such element exists");
            return it->second;
        }

        mapped_type& operator[](const key_type& key)
        {
            return ht_.find_or_inert(value_type(key, T())).second;
        }

        mapped_type& operator[](key_type&& key)
        {
            iterator it = ht_.find(key);
            if (it.node == nullptr)
                it = ht_.emplace_unique(mystl::move(key), T{}).first;
            return it->second;
        }

        size_type count(const key_type& key) const
        { return ht_.count(key); }

        iterator find(const key_type& key)
        { return ht_.find(key); }
        const_iterator find(const key_type& key) const
        { return ht_.find(key); }

        mystl::pair<iterator, iterator> equal_range(const key_type& key)
        { return ht_.equal_range(key); }
        mystl::pair<const_iterator, const_iterator> equal_range(const key_type& key) const
        { return ht_.equal_range(key); }

        size_type bucket_size() const noexcept
        { return ht_.bucket_size(); }

        size_type max_bucket_count() const noexcept
        { return ht_.max_bucket_count(); }

    public:
        bool operator==(const unordered_map& rhs)
        { return ht_ == rhs.ht_; }

        bool operator!=(const unordered_map& rhs)
        { return !(ht_ == rhs.ht_); }

    };

    template <class Key, class T, class HashFcn = mystl::hash<Key>,
            class EqualKey = mystl::equal_to<Key>>
    void swap(unordered_map<Key, T, HashFcn, EqualKey>& lhs,
              unordered_map<Key, T, HashFcn, EqualKey>& rhs)
    {
        lhs.swap(rhs);
    }
}

#endif //TINYSTL_UNORDERED_MAP_H
