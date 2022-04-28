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

        typedef slist_iterator<T, T&, T*>                   iterator;
        typedef slist_iterator<T, const T&, const T*>       const_iterator;

    private:
        typedef slist_node<T>                               list_node;
        typedef slist_node_base                             list_node_base;
        typedef allocator<list_node>                        list_node_allocator;
        typedef allocator<T>                                data_allocator;

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

    };
}

#endif //TINYSTL_SLIST_H
