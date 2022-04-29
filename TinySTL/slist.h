//
// Created by Cu1 on 2022/4/28.
//

#ifndef TINYSTL_SLIST_H
#define TINYSTL_SLIST_H

// 该头文件包含一个模板类 slist
// slist: 单向链表

#include <initializer_list>

#include "memory.h"
#include "iterator.h"
#include "functional.h"
#include "util.h"
#include "exceptdef.h"

namespace mystl
{
    struct slist_node_base
    {
        slist_node_base* next;
    };

    // 插入新节点
    inline slist_node_base* slist_make_link(slist_make_link* prev_node,
                                            slist_node_base* new_node)
    {
        new_node->next = prev_node->next;
        prev_node->next = new_node;
        return new_node;
    }

    inline slist_node_base* slist_previous(slist_node_base* head,
                                           const slist_node_base* node)
    {
        while (head && head->next != node)
            head = head->next;
        return head;
    }

    inline const slist_node_base* slist_previous(const slist_node_base* head,
                                                 const slist_node_base* node)
    {
        while (head && head->next != node)
            head = head->next;
        return head;
    }

    inline void slist_splice_after(slist_node_base* pos,
                                   slist_node_base* before_first,
                                   slist_node_base* before_last)
    {
        if (pos != before_first && pos != before_last)
        {
            slist_node_base* first = before_first->next;
            slist_node_base* after = pos->next;
            before_first->next = before_last->next;
            pos->next = first;
            before_last->next = after;
        }
    }

    inline slist_node_base* slist_reverse(slist_node_base* node)
    {
        slist_node_base* result = node;
        node = node->next;
        result->next = nullptr;
        while (node)
        {
            slist_node_base* next = node->next;
            node->next = result;
            result = node;
            node = next;
        }
        return result;
    }

    template <class T>
    struct slist_node : public slist_node_base
    {
        T data;
    };

    template <class T, class Ref, class Ptr>
    struct slist_iterator
    {
        typedef slist_iterator<T, T&, T*>               iterator;
        typedef slist_iterator<T, const T&, const T*>   const_iterator;
        typedef slist_iterator<T, Ref, Ptr>             self;

        typedef forward_iterator_tag                    iterator_category;
        typedef ptrdiff_t                               difference_type;
        typedef T                                       value_type;
        typedef Ref                                     reference;
        typedef Ptr                                     pointer;
        typedef slist_node_base                         node_base;
        typedef node_base*                              node_base_ptr;
        typedef slist_node<T>                           list_node;
        typedef list_node*                              list_node_ptr;

        node_base_ptr node;

        slist_iterator(list_node* x) : node(x)
        {
        }

        slist_iterator() : node(nullptr)
        {
        }

        slist_iterator(const iterator& it)
            : node(it.node)
        {
        }

        void incr() { node = node->next; }

        reference operator*() const
        { return dynamic_cast<list_node_ptr>(node)->data; }

        pointer operator->() const
        { return &(operator*()); }

        iterator& operator++()
        {
            incr();
            return *this;
        }

        iterator operator++(int)
        {
            auto tmp = *this;
            incr();
            return tmp;
        }
    };

    template <class T>
    class slist
    {
    public:

        typedef allocator<T>                                allocator_type;

        typedef typename allocator_type::value_type         value_type;
        typedef typename allocator_type::pointer            pointer;
        typedef typename allocator_type::const_pointer      const_pointer;
        typedef typename allocator_type::reference          reference;
        typedef typename allocator_type::const_reference    const_reference;
        typedef typename allocator_type::size_type          size_type;
        typedef typename allocator_type::difference_type    difference_type;

        typedef slist_iterator<T, T&, T*>                   iterator;
        typedef slist_iterator<T, const T&, const T*>       const_iterator;

    private:
        typedef slist_node<T>                               list_node;
        typedef slist_node_base                             list_node_base;
        typedef allocator<list_node>                        list_node_allocator;
        typedef allocator<T>                                data_allocator;

        list_node_base* head_;
        size_type       size_;

    private:

        template <class ...Args>
        list_node* create_node(Args&& ...args)
        {
            list_node* p = list_node_allocator::allocate(1);
            try {
                data_allocator::construct(&(p->data), mystl::forward<Args>(args)...);
                p->next = nullptr;
            }
            catch (...) {
                list_node_allocator::deallocate(p);
                throw ;
            }
            return p;
        }


        static void destory_node(list_node* node)
        {
            mystl::destroy(&node->data);
            list_node_allocator::deallocate(node);
        }

        void fill_initialize(size_type n, const value_type& x)
        {
            head_ = create_node(0);
            head_->next = nullptr;
            size_ = 0;
            try {
                insert_after_fill(head, n, x);
            }
            catch (...) {
                clear();
            }
        }

        template <class InputIter>
        void range_initialize(InputIter first, InputIter last)
        {
            head_ = create_node(0);
            head_->next = nullptr;
            size_ = 0;
            try {
                insert_after_range(head, first, last);
            }
            catch (...) {
                clear();
            }
        }

    public:
        slist() : size_(0)
        {
            head_ = create_node(0);
            head_->next = nullptr;
        }

        explicit slist(size_type n)
        { fill_initialize(n, value_type()); }

        slist(size_type n, const value_type& value)
        { fill_initialize(n, value); }

        template <class InputIter, typename std::enable_if<
                  mystl::is_input_iterator<InputIter>::value, int>::type = 0>
        slist(InputIter first, InputIter last)
        { range_initialize(first, last); }

        slist(std::initializer_list<value_type> ilist)
        { range_initialize(ilist.begin(), ilist.end()); }

        slist(const slist& other)
        { range_initialize(other.begin(), other.end()); }

        slist(slist&& other) : head_(other.head_), size_(other.size_)
        {
            other.head_ = nullptr,
            other.size_ = 0;
        }

        slist& operator=(const slist& rhs)
        {
            if (this != &rhs)
            {
                clear();
                assign(rhs.begin(), rhs.end());
            }
            return *this;
        }

        slist& operator=(slist&& rhs)
        {
            clear();
            splice(head_->next, rhs);
            return *this;
        }

        slist& operator=(std::initializer_list<value_type> ilist)
        {
            list tmp(ilist.begin(), ilist.end());
            swap(tmp);
            return *this;
        }

        ~slist()
        {
            if (head_)
            {
                clear();
                list_node_allocator::deallocate(head_);
                head_ = nullptr;
                size_ = 0;
            }
        }

    public:
        iterator        begin()           noexcept
        { return head_->next; }

        const_iterator  begin() const     noexcept
        { return head_->next; }

        iterator        end()             noexcept
        { return nullptr; }

        const_iterator  end()   const     noexcept
        { return nullptr;}

        const_iterator cbegin() const noexcept
        { return begin(); }
        const_iterator cend()   const noexcept
        { return end(); }

        size_type size() const noexcept { return size_; }
        size_type max_size() const noexcept
        { return static_cast<size_type>(-1); }

        bool empty() const noexcept
        { return head_->next == nullptr; }

        void swap(slist& other)
        {
            mystl::swap(head_, other.head_);
            mystl::swap(size_, other.size_);
        }

    public:
        reference front()
        { return dynamic_cast<list_node*>(head_->next)->data; }

        const_reference front() const
        { return dynamic_cast<list_node*>(head_->next)->data; }

        template <class ...Args>
        void emplace_front(Args&& ...args)
        {
            slist_make_link(head_, create_node(mystl::forward<Args>(args)...));
        }

        void push_front(const value_type& value)
        { slist_make_link(head_, create_node(value)); }

        void push_front(value_type&& value)
        { emplace_front(mystl::move(value)); }

        void pop_front()
        {
            list_node* node = dynamic_cast<list_node*>(head_->next);
            head_->next = node->next;
            destory_node(node);
        }

        iterator previous(const_iterator pos)
        {
            return iterator(dynamic_cast<list_node*>(slist_previous(head_, pos.node)));
        }

        const_iterator previous(const_iterator pos) const
        {
            return const_iterator(dynamic_cast<list_node*>(slist_previous(head_, pos.node)));
        }

    private:
        list_node* insert_after(list_node_base* pos, const value_type& value)
        { return dynamic_cast<list_node*>(slist_make_link(pos, create_node(value)));}

        list_node* emplace_after(list_node_base* pos, value_type&& value)
        { return dynamic_cast<list_node*>(slist_make_link(pos, create_node(mystl::move(value)))); }

        void insert_after_fill(list_node_base* pos,
                               size_type n, const value_type& value)
        {
            for (size_type i = 0; i < n; ++i)
                pos = slist_make_link(pos, create_node(value));
        }

        template <class InputIter>
        void insert_after_range(list_node_base* pos, InputIter first, InputIter last)
        {
            for (; first != last; ++first)
                pos = slist_make_link(pos, create_node(*first));
        }

        list_node_base* erase_after(list_node_base* pos)
        {
            auto next = dynamic_cast<list_node*>(pos->next);
            auto next_next = next->next;
            pos->next = next_next;
            destory_node(next);
            return next_next;
        }

        // 删除 (before_first, last_node) 中间元素, 返回 last_node
        list_node_base* erase_after(list_node_base* before_first,
                                    list_node_base* last_node)
        {
            for (auto cur = dynamic_cast<list_node*>(before_first->next); cur != last_node; )
            {
                list_node* tmp = cur;
                cur = dynamic_cast<list_node*>(cur->next);
                destory_node(tmp);
            }
            before_first->next = last_node;
            return last_node;
        }

    public:
        // 在 pos 后插入值 value
        iterator insert_after(iterator pos, const value_type& value)
        {
            return insert_after(pos.node, value);
        }

        iterator insert_after(iterator pos, value_type&& value)
        {
            return emplace_after(pos.node, mystl::move(value));
        }

        iterator insert_after(iterator pos)
        {
            return insert_after(pos.node, value_type());
        }

        void insert_after(iterator pos, size_type n, const value_type& value)
        {
            insert_after_fill(pos.node, n, value);
        }

        template <class InputIter>
        void insert_after(iterator pos, InputIter first, InputIter last)
        {
            insert_after_range(pos.node, first, last);
        }

        iterator insert(iterator pos, const value_type& value)
        {
            return insert_after(slist_previous(head_, pos.node), value);
        }
    };
}

#endif //TINYSTL_SLIST_H
