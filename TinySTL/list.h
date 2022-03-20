//
// Created by Cu1 on 2022/3/19.
//

#ifndef TINYSTL_LIST_H
#define TINYSTL_LIST_H

// 该头文件包含了模板类 list
// list: 双向链表

#include <initializer_list>

#include "iterator.h"
#include "memory.h"
#include "functional.h"
#include "util.h"
#include "exceptdef.h"

namespace tstl
{
    template <class T> struct list_node_base;
    template <class T> struct list_node;


    template <class T>
    struct node_traits
    {
        typedef list_node_base<T>*  base_ptr;
        typedef list_node<T>*       node_ptr;
    };

    template <class T>
    struct list_node_base
    {
        typedef typename node_traits<T>::base_ptr base_ptr;
        typedef typename node_traits<T>::node_ptr node_ptr;

        base_ptr prev; // 前驱节点
        base_ptr next; // 后继节点

        node_ptr as_node() { return static_cast<node_ptr>(self());}

        void unlink() { prev = next = self(); }

        base_ptr self() { return static_cast<base_ptr>(&*this); }
    };

    template <class T>
    struct list_node : public list_node_base<T>
    {
        typedef typename node_traits<T>::base_ptr base_ptr;
        typedef typename node_traits<T>::node_ptr node_ptr;

        // 数据
        T value;

        list_node() = default;
        list_node(const T& v)
            : value(v) {}

        base_ptr as_base() { return static_cast<base_ptr>(&*this); }
        node_ptr self() { return static_cast<node_ptr>(&*this); }
    };

    // 迭代器
    template <class T>
    struct list_iterator : public tstl::iterator<bidirectional_iterator_tag, T>
    {
        typedef T                                 value_type;
        typedef T*                                pointer;
        typedef T&                                reference;
        typedef typename node_traits<T>::base_ptr base_ptr;
        typedef typename node_traits<T>::node_ptr node_ptr;
        typedef list_iterator<T>                  self;

        base_ptr node_; // 指向当前节点

        // 构造函数
        list_iterator() = default;
        list_iterator(base_ptr x)
            : node_(x) {}
        list_iterator(node_ptr x)
            : node_(x->as_base()) {}
        list_iterator(const list_iterator& rhs)
            : node_(rhs.node_) {}

        // 重载操作符
        reference operator*() const { return (node_->as_node())->value; }
        pointer   operator->() const { return &(operator*()); }

        self& operator++()
        {
            TSTL_DEBUG(node_ != nullptr);
            node_ = node_->next;
            return *this;
        }
        self operator++(int)
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
        self& operator--()
        {
            TSTL_DEBUG(node_ != nullptr);
            node_ = node_->prev;
            return node_;
        }
        self operator--(int)
        {
            auto tmp = *this;
            --*this;
            return tmp;
        }

        // 重载比较操作符
        bool operator==(const self& rhs) const { return node_ == rhs.node_; }
        bool operator!=(const self& rhs) const { return node_ != rhs.node_; }
    };

    // 模板类 list
    // 模板参数 T 代表数据类型
    template <class T>
    class list
    {
    public:
        // list 的嵌套类型定义
        typedef tstl::allocator<T>                          allocator_type;
        typedef tstl::allocator<T>                          data_allocator;
        typedef tstl::allocator<list_node_base<T>>          base_allocator;
        typedef tstl::allocator<list_node<T>>               node_allocator;

        typedef typename allocator_type::value_type         value_type;
        typedef typename allocator_type::pointer            pointer;
        typedef typename allocator_type::const_pointer      const_pointer;
        typedef typename allocator_type::reference          reference;
        typedef typename allocator_type::const_reference    const_reference;
        typedef typename allocator_type::size_type          size_type;
        typedef typename allocator_type::difference_type    difference_type;

        typedef list_iterator<T>                            iterator;
        typedef const list_iterator<T>                      const_iterator;
        typedef tstl::reverse_iterator<iterator>            reverse_iterator;
        typedef tstl::reverse_iterator<const_iterator>      const_reverse_iterator;

        typedef typename node_traits<T>::base_ptr           base_ptr;
        typedef typename node_traits<T>::node_ptr           node_ptr;

        allocator_type get_allocator() { return node_allocator(); }

    private:
        base_ptr node_; // 空节点
        size_type size_; // 链表大小

    public:
        // 构造 复制, 移动, 析构函数
        list()
        { fill_init(0, value_type()); }

        explicit list(size_type n)
        { fill_init(n,value_type()); }

        list(size_type n, const T& value)
        { fill_init(n, value); }

        template<class Iter, typename std::enable_if<
                tstl::is_input_iterator<Iter>::value, int>::type = 0>
        list(Iter first, Iter last)
        { copy_init(first, last); }

        list(std::initializer_list<T> ilist)
        { copy_init(ilist.begin(), ilist.end()); }

        list(const list& rhs)
        { copy_init(rhs.cbegin(), rhs.cend()); }

        list(list&& rhs) noexcept
            : node_(rhs.node_), size_(rhs.size_)
        {
            rhs.node = nullptr;
            rhs.size = 0;
        }

        list& operator=(const list& rhs)
        {
            if (this != &rhs)
            {
                assign(rhs.begin(), rhs.end());
            }
            return *this;
        }

        list& operator=(list&& rhs) noexcept
        {
            clear();
            splice(end(), rhs);
            return *this;
        }

        list& operator=(std::initializer_list<T> ilist)
        {
            list tmp(ilist.begin(), ilist.end());
            swap(tmp);
            return *this;
        }
    };
}


#endif //TINYSTL_LIST_H
