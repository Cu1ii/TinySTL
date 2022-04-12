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
    template <class Key, class T, class Compare = mystl::less<Value>>
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
                : public mystl::unarg_function<value_type, value_type.first>
        {
            friend class map<Key, T, Compare>;

        public:
            result_type operator()(const value_type& value) const
            {
                return value.first;
            }

        };

    private:
        typedef mystl::rb_tree<key_type, value_type,
                               select1st<value_type>, key_compare> rep_type;
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

    };
}

#endif //TINYSTL_MAP_H
