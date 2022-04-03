//
// Created by Cu1 on 2022/4/2.
//

#ifndef TINYSTL_RB_TREE_H
#define TINYSTL_RB_TREE_H


// 该模板类为 rb_tree (红黑树)

#include <initializer_list>
#include <cassert>

#include "iterator.h"
#include "memory.h"
#include "type_traits.h"
#include "exceptdef.h"

namespace mystl
{
    typedef bool rb_tree_color_type;
    const rb_tree_color_type rb_tree_red   = false;
    const rb_tree_color_type rb_tree_black = true;

    struct rb_tree_node_base
    {
        typedef rb_tree_color_type color_type;
        typedef rb_tree_node_base* base_ptr;

        color_type  color;
        base_ptr    parent;
        base_ptr    left;
        base_ptr    right;

        static base_ptr minimum(base_ptr x)
        {
            while (nullptr != x->left) x = x->left;
            return x;
        }

        static base_ptr maximum(base_ptr x)
        {
            while (nullptr != x->right) x = x->right;
            return x;
        }
    };

    template <class Value>
    struct rb_tree_node : public rb_tree_node_base
    {
        typedef rb_tree_node<Value>*  link_type;
        Value value_field;
    };

    struct rb_tree_base_iterator
    {
        typedef rb_tree_node_base::base_ptr base_ptr;
        typedef bidirectional_iterator_tag  iterator_category;
        typedef ptrdiff_t                   difference_type;

        base_ptr node;

        void increment()
        {
            if (nullptr != node->right)
            {
                node = node->right;
                while (nullptr != node->left)
                    node = node->left;
            }
            else
            {
                base_ptr y = node->parent;
                while (node == y->right)
                {
                    node = y;
                    y = y->parent;
                }
                if (node->right != y)
                    node = y;
            }
        }

        void decrement()
        {
            // 如果到达 end();
            if (node->color == rb_tree_red && node->parent->parent == node)
                node = node->right;
            else if (nullptr != node->left)
            {
                base_ptr y = node->left;
                while (nullptr != y->right)
                    y = y->right;
                node = y;
            }
            else
            {
                base_ptr y = node->parent;
                while (node == y->left)
                {
                    node = y;
                    y = y->parent;
                }
                node = y;
            }
        }

        bool operator==(const rb_tree_iterator_base& rhs) { return node == rhs.node; }
        bool operator!=(const rb_tree_iterator_base& rhs) { return node != rhs.node; }

    };

    template <class Value, class Ref, class Ptr>
    struct rb_tree_iterator : public rb_tree_base_iterator
    {
        typedef Value                                               value_type;
        typedef Ref                                                 reference;
        typedef Ptr                                                 pointer;
        typedef rb_tree_iterator<Value, Value&, Value*>             iterator;
        typedef rb_tree_iterator<Value, const Value&, const Value*> const_iterator;
        typedef rb_tree_iterator<Value, Ref, Ptr>                   self;
        typedef rb_tree_node<Value>*                                link_type;

        rb_tree_iterator() = default;
        rb_tree_iterator(link_type x)
            : node(x)
        {
        }
        rb_tree_iterator(const iterator& it)
            : node(it.node)
        {
        }

        reference operator*()   const { return link_type(node)->value_field; }
        pointer   operator->)() const { return &(operator*()); }

        iterator& operator++() { increment(); return *this; }
        iterator operator++(int)
        {
            iterator tmp = *this;
            increment(); 
            return tmp;
        }
        iterator& operator--() { decrement(); return *this;}
        iterator  operator--(int)
        {
            iterator tmp = *this;
            decrement();
            return *this;
        }
    };

    inline bidirectional_iterator_tag
    iterator_category(const rb_tree_base_iterator&)
    { return bidirectional_iterator_tag(); }

    inline rb_tree_base_iterator::difference_type*
    distance_type(const rb_tree_base_iterator&)
    { return (rb_tree_base_iterator::difference_type*) 0; }

    template <class Value, class Ref, class Ptr>
    inline Value* value_type(const rb_tree_iterator<Value, Ref, Ptr>&)
    { return (Value*)0; }



    /*---------------------------------------*\
    |       p                         p       |
    |      / \                       / \      |
    |     x   d    rotate left      y   d     |
    |    / \       ===========>    / \        |
    |   a   y                     x   c       |
    |      / \                   / \          |
    |     b   c                 a   b         |
    \*---------------------------------------*/
    // 左旋，参数一为左旋点，参数二为根节点
    // 注释摘自项目 MyTinySTL
    inline void
    rb_tree_rotate_left(rb_tree_node_base* x, rb_tree_node_base*& root)
    {
        rb_tree_node_base* y = x->right;
        x->right = y->left;
        if (nullptr != y->left)
            y->left->parent = x;
        y->parent = x->parent;

        if (x == root)
            root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    /*----------------------------------------*\
    |     p                         p          |
    |    / \                       / \         |
    |   d   x      rotate right   d   y        |
    |      / \     ===========>      / \       |
    |     y   a                     b   x      |
    |    / \                           / \     |
    |   b   c                         c   a    |
    \*----------------------------------------*/
    // 右旋，参数一为右旋点，参数二为根节点
    // 注释摘自 github MyTinySTL
    inline void
    rb_tree_rotate_right(rb_tree_node_base* x, rb_tree_node_base*& root)
    {
        rb_tree_node_base* y = x->left;
        x->left = y->right;
        if (nullptr != y->right)
            y->right->parent = x;
        y->parent = x->parent;
        if (x == root)
            root = y;
        else if (x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
        y->right = x;
        x->parent = y;
    }



    // 添加节点后对红黑树的颜色进行调整
    /**
     * 对于新插入节点后的调整, 有下面几种情况:
     *  Case 1: 当前节点的父亲节点为红色, 且当前节点的叔叔节点也是红色
     *      (1) 将父亲节点变为黑色
     *      (2) 将叔叔节点变为黑色
     *      (3) 将祖父节点变为红色
     *      (4) 将祖父节点设置为当前节点, 并对新的当前节点进行操作
     *
     *  Case 2: 当前节点以及父亲节点为红色, 叔叔节点为黑色且当前节点为父节点的右儿子
     *      (1)  将父节点作为当前节点, 对父节点进行左旋操作
     *      (2) 将新的当前节点进行左旋
     *
     *  Case 3: 当前节点以及父亲节点为红色, 叔叔节点为黑色且当前节点为父节点的左儿子
     *      (1) 将父亲节点设置为黑色
     *      (2) 将当前节点的祖父节点设置为红色
     *      (3) 将祖父节点进行右旋
     */
    inline void
    rb_tree_reblance(rb_tree_node_base* x, rb_tree_node_base*& root)
    {
        x->color = rb_tree_red;
        while (x != root && x->parent->color == rb_tree_red)
        {
            if (x->parent == x->parent->parent->left)
            {
                auto y = x->parent->parent->right;
                // case 1
                if (y && y->color == rb_tree_red)
                {
                    x->parent->color = rb_tree_black;
                    y->color = rb_tree_black;
                    x->parent->parent->color = rb_tree_red;
                    x = x->parent->parent;
                }
                else
                {
                    // case 2
                    if (x == x->parent->right)
                    {
                        x = x->parent;
                        rb_tree_rotate_left(x, root);
                    }
                    // case 3, 只要不是 case 1, 无论有没有经过 case 2, 都会变成 case 3
                    x->parent->color = rb_tree_black;
                    x->parent->parent->color = rb_tree_red;
                    rb_tree_rotate_right(x->parent->parent, root);
                }
            }
            // 如果父节点是祖父节点的右儿子, 操作步骤是一样的, 不过有关旋转的方向反一下就可以
            else
            {
                auto y = x->parent->parent->left;
                if (y && y->color == rb_tree_red)
                {
                    x->parent->color = rb_tree_black;
                    y->color = rb_tree_black;
                    x->parent->parent->color = rb_tree_red;
                    x = x->parent->parent;
                }
                else
                {
                    if (x == x->parent->left)
                    {
                        x = x->parent;
                        rb_tree_rotate_right(x, root);
                    }
                    x->parent->color = rb_tree_black;
                    x->parent->parent->color = rb_tree_red;
                    rb_tree_rotate_left(x->parent->parent, root);
                }
            }
        }
        // 由于旋转后根节点可能会变为红色, 这里将根节点染为黑色
        root->color = rb_tree_black;
    }

    // 删除节点后使得 rb_tree 重新满足红黑树条件


    inline rb_tree_node_base*
    rb_tree_rebalance_for_erase(rb_tree_node_base* z,
                                rb_tree_node_base* root,
                                rb_tree_node_base*& leftmost,
                                rb_tree_node_base*& rightmost)
    {
        rb_tree_node_base* y = z;
        rb_tree_node_base* x = 0;
        rb_tree_node_base* x_parent = 0;
        // 如果左右儿子至少有一个为空, 则直接利用子节点代替要删除的节点
        if (nullptr == y->left)
            x = y->right;
        else if (nullptr == y->right)
            x = y->left;
        else
        {
            // 否则就找右子树的最左节点与要删除节点交换
            // 由于该节点一定是只有右儿子的节点, 所以交换完后直接删除即可
            y = y->right;
            while (nullptr == y->left)
                y = y->left;
            x = y->right;
        }
        //如果是两非空子节点的情况, 此时 y 为 要删除节点的后继
        if (y != z)
        {
            // 先将要删除节点的左儿子的父节点指向 y
            z->left->parent = y;
            y->left = z->left;
            // 如果 y 不是要删除节点 z 的右儿子
            if (y != z->right)
            {
                x_parent = y->parent;
                if (x) x->parent = y->parent;
                y->parent->left = x;
                y->right = z->right;
                z->right->parent = y;
            } // 如果 y 是要删除节点 z 的右儿子
            else
                x_parent = y;
            if (root == z)
                root = y;
            else if (z->parent->left == z)
                z->parent->left = y;
            else
                z->parent->right = y;
            y->parent = z->parent;
            mystl::swap(y->color, z->color);
            y = z;
        }
        else
        {
            x_parent = y->parent;
            if (x) x->parent = y->parent;
            if (root == z)
                root = x;
            else if (z->parent->left = z)
                z->parent->left = x;
            else
                z->parent->right = x;
            if (leftmost == z)
                if (nullptr == z->right)
                    leftmost = z->parent;
                else
                    leftmost = rb_tree_node_base::minimum(x);


            if (rightmost == z)
                if (nullptr == z->left)
                    rightmost = z->parent;
                else
                    rightmost = rb_tree_node_base::maximum(x);
        }

        // 开始修复因删除而破坏的红黑树性质
        if (y->color != rb_tree_red)
        {
            while (x != root && (nullptr == x || x->color == rb_tree_black))
                if (x == x_parent->left)
                {
                    auto w = x_parent->right;
                    if (w->color == rb_tree_red)
                    {
                        w->color = rb_tree_black;
                        x_parent->color = rb_tree_red;
                        rb_tree_rotate_left(x_parent, root);
                        w = x_parent->right;
                    }
                    // 如果第一步执行了, 那么就满足 w 节点是黑色
                    // 反之, 则说明 w 本来就是黑色, 所以只需要判断子节点符不符合条件即可
                    if ((nullptr == w->left || w->left->color == rb_tree_black) &&
                         nullptr == w->right || w->right->color == rb_tree_black)
                    {
                        w->color = rb_tree_red;
                        x = x_parent;
                        x_parent = x_parent->parent;
                    }
                    else
                    {
                        if (nullptr == w->right || w->right->color == rb_tree_black)
                        {
                            w->color = rb_tree_red;
                            rb_tree_rotate_right(w, root);
                            w = x_parent->right;
                        }
                        w->color = x_parent->color;
                        x_parent->color = rb_tree_black;
                        if (w->right) w->right->color = rb_tree_black;
                        rb_tree_rotate_left(x_parent, root);
                        break;
                    }
                }
                else
                {
                    auto w = x_parent->left;
                    if (w->color == rb_tree_red)
                    {
                        w->color = rb_tree_black;
                        x_parent->color = rb_tree_red;
                        rb_tree_rotate_right(x_parent, root);
                        w = x_parent->left;
                    }
                    if ((nullptr == w->right || w->right->color == rb_tree_black) &&
                        (nullptr == w->left  || w->left->color == rb_tree_black))
                    {
                        w->color = rb_tree_red;
                        x = x_parent;
                        x_parent = x_parent->parent;
                    }
                    else
                    {
                        if (nullptr == w->left || w->left->color == rb_tree_black)
                        {
                            if (w->right)
                                w->right->color = rb_tree_black;
                            w->color = rb_tree_red;
                            w = x_parent->left;
                        }
                        w->color = x_parent->color;
                        x_parent->color = rb_tree_black;
                        if (w->left)
                            w->left->color = rb_tree_black;
                        rb_tree_rotate_right(x_parent, root);
                        break;
                    }
                }
            if (x)
                x->color = rb_tree_black;
        }
        return y;
    }

    template <class Key, class Value, class KeyOfValue, class Compare>
    class rb_tree
    {
    protected:
        typedef rb_tree_color_type                      color_type;
        typedef rb_tree_node_base*                      base_ptr;
        typedef rb_tree_node<Value>                     rb_tree_node;


        typedef mystl::allocator<T>                     allocator_type;
        typedef mystl::allocator<T>                     data_allocator;
        typedef mystl::allocator<rb_tree_node_base>     base_allocator;
        typedef mystl::allocator<rb_tree_node>          node_allocator;

    public:
        typedef Key                                      key_type;
        typedef Value                                    value_type;
        typedef rb_tree_node*                            link_type;

        typedef typename allocator_type::pointer         pointer;
        typedef typename allocator_type::const_pointer   const_pointer;
        typedef typename allocator_type::reference       reference;
        typedef typename allocator_type::const_reference const_reference;
        typedef typename allocator_type::size_type       size_type;
        typedef typename allocator_type::difference_type difference_type;

        typedef rb_tree_iterator<value_type, reference, pointer>                iterator;
        typedef rb_tree_iterator<value_type, const_reference, const_pointer>    const_iterator;
        typedef mystl::reverse_iterator<iterator>                               reverse_iterator;
        typedef mystl::reverse_iterator<const_iterator>                         const_reverse_iterator;

    protected:
        link_type get_node() { return node_allocator::allocate(1); }
        void put_node(link_type p) { node_allocator::deallocate(p); }

        link_type create_node(const value_type& value)
        {
            link_type tmp = get_node();
            try
            {
                data_allocator::construct(mystl::address_of(tmp->value_field), value);
                tmp->left = nullptr;
                tmp->right = nullptr;
                tmp->parent = nullptr;
            }
            catch (...)
            {
                node_allocator::deallocate(tmp);
                throw ;
            }
            return tmp;
        }

        template <class ...Args>
        link_type create_node(Args&& ...args)
        {
            link_type tmp = get_node();
            try
            {
                data_allocator::construct(mystl::address_of(tmp->value_field),
                                          mystl::forward<Args>(args)...);
                tmp->left = nullptr;
                tmp->right = nullptr;
                tmp->parent = nullptr;
            }
            catch (...)
            {
                node_allocator::deallocate(tmp);
                throw ;
            }
            return tmp;
        }

        link_type clone_type(link_type x)
        {
            link_type tmp = create_node(x->value_field);
            tmp->color = x->color;
            tmp->left = nullptr;
            tmp->right = nullptr;
            return tmp;
        }

        void destroy_node(link_type p)
        {
            data_allocator::destroy(&(p->value_field));
            node_allocator::deallocate(p);
        }

    protected:
        size_type node_count;
        link_type header;
        Compare key_compare;

        link_type& root()       const { return static_cast<link_type&>(header->parent); }
        link_type& leftmost()   const { return static_cast<link_type&>(header->left); }
        link_type& rightmost()  const { return static_cast<link_type&>(header->right); }

        static link_type& left(link_type x) { return static_cast<link_type&>(x->left); }
        static link_type& right(link_type x) { return static_cast<link_type&>(x->right);; }
        static link_type& parent(link_type x) { return static_cast<link_type&>(x->parent); }
        static reference  value(base_ptr x) { return (static_cast<link_type>(x))->value_field; }
        static const Key& key(base_ptr x) const { return KeyOfValue()(value(link_type(x))); }
        static color_type& color(base_ptr x) { return (color_type&)(link_type(x)->color); }

        static link_type minimum(link_type x)
        {
            return static_cast<link_type>(rb_tree_node_base::minimum(x));
        }

        static link_type maximun(link_type x)
        {
            return static_cast<link_type>(rb_tree_node_base::maximum(x));
        }

        void rb_tree_init()
        {
            header = get_node();
            header->color = rb_tree_red;
            root() = nullptr;
            leftmost() = header;
            rightmost() = header;
        }

        void reset()
        {
            header = nullptr;
            node_count = 0;
        }


    public:
        rb_tree() :node_count(0), key_compare()
        { rb_tree_init(); }

        rb_tree(const rb_tree<Key, Value, KeyOfValue, Compare>& other)
            : node_count(other.node_count), key_compare(other.key_compare)
        {
            rb_tree_init();
            if (0 != other.node_count)
            {
                // TODO __cpoy()
                root() = __copy(other.root(), header);
                leftmost() = rb_tree::minimum(root());
                rightmost() = rb_tree::maximun(root());
            }
        }
        rb_tree(rb_tree<Key, Value, KeyOfValue, Compare>&& other) noexcept
            : header(mystl::move(other.header)),
              node_count(other.node_count),
              key_compare(other.key_compare)
        {
            other.reset();
        }

        rb_tree& operator=(const rb_tree<Key, Value, KeyOfValue, Compare>& rhs)
        {
            if (this != &rhs)
            {
                // TODO clear()
                clear();
                if (0 != rhs.node_count)
                {
                    root() = __copy(rhs.root(), header);
                    leftmost() = rb_tree::minimum(root());
                    rightmost() = rb_tree::maximum(root());
                }
                node_count = rhs.node_count;
                key_compare = rhs.key_compare;
            }
            return *this;
        }

        rb_tree& operator=(rb_tree<Key, Value, KeyOfValue, Compare>&& rhs)
        {
            clear();
            header = mystl::move(rhs.header);
            node_count = rhs.node_count;
            key_compare = rhs.key_compare;
            rhs.reset();
            return *this;
        }

        ~rb_tree() { clear(); }

    public:
        // 迭代器
        iterator                begin()             noexcept { return leftmost(); }
        const_iterator          begin()       const noexcept { return leftmost(); }
        iterator                end()               noexcept { return header; }
        const_iterator          end()         const noexcept { return header; }

        reverse_iterator        rbegin()            noexcept
        { return reverse_iterator(end()); }
        const_reverse_iterator  rbegin()      const noexcept
        { return const_reverse_iterator(end()); }
        reverse_iterator        rend()              noexcept
        { return reverse_iterator(begin()); }
        const_reverse_iterator  rend()        const noexcept
        { return const_reverse_iterator(begin()); }

        const_iterator          cbegin()      const noexcept
        { return begin(); }
        const_iterator          cend()        const noexcept
        { return end(); }
        const_reverse_iterator  crbegin()     const noexcept
        { return rbegin(); }
        const_reverse_iterator  crend()       const noexcept
        { return rend(); }

        // 容器相关操作
        Compare     key_compare() const          { return key_compare; }
        bool        empty()       const noexcept { return node_count == 0;}
        size_type   size()        const noexcept { return node_count; }
        size_type   max_size()    const noexcept { return static_cast<size_type>(-1); }
        void swap(rb_tree<Key, Value, KeyOfValue, Compare>& rhs) noexcept
        {
            if (this != &rhs)
            {
                mystl::swap(header, rhs.header);
                mystl::swap(node_count, rhs.node_count);
                mystl::swap(key_compare, rhs.key_compare);
            }
        }

    public:
        // 插入删除相关操作

        // emplace

        // TODO iterator emplace_multi(Args&& ...args);
        template <class... Args>
        iterator emplace_multi(Args&& ...args);

        // TODO mystl::pair<iterator, bool> emplace_unique(Args ...args);
        template<class ...Args>
        mystl::pair<iterator, bool> emplace_unique(Args ...args);

        // TODO iterator emplace_multi_use_hint(iterator hint, Args&& ...args);
        template<class ...Args>
        iterator emplace_multi_use_hint(iterator hint, Args&& ...args);

        //TODO iterator emplace_unique_use_hint(iterator hint, Args&& ...args);
        template<class ...Args>
        iterator emplace_unique_use_hint(iterator hint, Args&& ...args);

        // insert

        // TODO insert_multi(const value_type& value);
        iterator insert_multi(const value_type& value);

        iterator insert_multi(value_type&& value)
        {
            return emplace_multi(mystl::move(value));
        }

        iterator insert_multi_use_hint(iterator hint, const value_type& value)
        {
            return emplace_multi_use_hint(hint, value);
        }

        iterator insert_multi_use_hint(iterator hint, value_type&& value)
        {
            return emplace_multi_use_hint(hint, mystl::move(value));
        }

        template <class InputIter>
        void    insert_multi(InputIter first, InputIter last)
        {
            size_type  n = mystl::distance(first, last);
            THROW_LENGTH_ERROR_IF(node_count > max_size() - n,
                                  "rb_tree<Key, Value, KeyOfValue, Compare>'s size too big");
            for (; n > 0; --n, ++first)
                insert_multi(end(), *first);
        }

        // TODO  mystl::pair<iterator, bool> insert_unique(const value_type& value);
        mystl::pair<iterator, bool> insert_unique(const value_type& value);

        mystl::pair<iterator, bool> insert_unique(value_type&& value)
        {
            emplace_unique(mystl::move(value));
        }

        template <class InputIter>
        void    insert_unique(InputIter first, InputIter last)
        {
            size_type  n = mystl::distance(first, last);
            THROW_LENGTH_ERROR_IF(node_count > max_size() - n,
                                  "rb_tree<Key, Value, KeyOfValue, Compare>'s size too big");
            for (; n > 0; --n, ++first)
                insert_unique(end(), *first);
        }

        // erase

        // TODO  iterator erase(iterator hint);
        iterator  erase(iterator hint);

        // TODO size_type erase_multi(const key_type& key);
        size_type erase_multi(const key_type& key);

        // TODO size_type erase_unique(const key_type& key);
        size_type erase_unique(const key_type& key);

        // TODO void erase(iterator first, iterator last);
        void      erase(iterator first, iterator last);

        // TODO void clear();
        void      clear();

    public:
        // rb_tree 相关操作

        // TODO iterator find(const key_type& key);
        iterator find(const key_type& key);

        // TODO const_iterator find(const key_type& key) const;
        const_iterator find(const key_type& key) const;

        size_type count_multi(const key_type& key) const
        {
            // TODO equal_range_multi
            auto p = equal_range_multi(key);
            return static_cast<size_type>(mystl::distance(p.first, p.second));
        }

        size_type count_unique(const key_type& key) const
        { return find(key) == end() ? 0 : 1; }

        // TODO iterator lower_bound(const key_type& key);
        iterator lower_bound(const key_type& key);

        // TODO const_iterator lower_bound(const key_type& key) const;
        const_iterator lower_bound(const key_type& key) const;

        // TODO iterator upper_bound(const key_type& key);
        iterator upper_bound(const key_type& key);

        // TODO const_iterator upper_bound(const key_type& key);
        const_iterator upper_bound(const key_type& key);


        mystl::pair<iterator, iterator>
        equal_range_multi(const key_type& key)
        {
            return mystl::pair<iterator, iterator>(lower_bound(key), upper_bound(key));
        }

        mystl::pair<const_iterator, const_iterator>
        equal_range_multi(const key_type& key) const
        {
            return mystl::pair<const_iterator, const_iterator>(lower_bound(key), upper_bound(key));
        }

        mystl::pair<iterator, iterator>
        equal_range_unique(const key_type& key)
        {
            auto it = find(key);
            auto nex = it;
            return it == end() ? mystl::make_pair(it, it) : make_pair(it, ++nex);
        }

        mystl::pair<const_iterator, const_iterator>
        equal_range_unique(const key_type& key) const
        {
            const_iterator it = find(key);
            auto nex = it;
            return it == end() ? mystl::make_pair(it, it) : make_pair(it, ++nex);
        }

        // get insert pos

        // TODO get_insert_multi_pos
        mystl::pair<link_type, bool>
        get_insert_multi_pos(const key_type& key);

        // TODO get_insert_unique_pos
        mystl::pair<mystl::pair<link_type, bool>, bool>
        get_insert_unique_pos(const key_type& key);

        // insert value / insert node

        // TODO insert_value_at
        iterator insert_value_at(link_type x, const value_type& value, bool add_to_left);
        // TODO insert_node_at
        iterator insert_node_at(link_type x, link_type node, bool add_to_left);

        // TODO insert_multi_use_hint
        iterator insert_multi_use_hint(iterator hint, key_type key, link_type node);
        // TODO insert_unique_use_hint
        iterator insert_unique_use_hint(iterator hint, key_type key, link_type node);

        // TODO __copy()
        link_type __copy(link_type x, link_type p);

        // TODO erase_since
        void erase_since(link_type x);

    };

    /****************************************************************************/


    template <class Key, class Value, class KeyOfValue, class Compare>
    template <class ...Args>
    typename rb_tree<Key, Value, KeyOfValue, Compare>::iterator
    rb_tree<Key, Value, KeyOfValue, Compare>::
    emplace_multi(Args &&args...)
    {
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1,
                              "rb_tree<Key, Value, KeyOfValue, Compare>'s size too big");
        link_type pos = create_node(mystl::forward<Args>(args)...);
        auto res = get_insert_multi_pos(KeyOfValue()(pos->value_field));
        return insert_node_at(res.first, pos, res.second);
    }

    template <class Key, class Value, class KeyOfValue, class Compare>
    template <class ...Args>
    mystl::pair<typename rb_tree<Key, Value, KeyOfValue, Compare>::iterator, bool>
    rb_tree<Key, Value, KeyOfValue, Compare>::
    emplace_unique(Args &&args...)
    {
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1,
                              "rb_tree<T, Comp>'s size too big");
        link_type pos = create_node(mystl::forward<Args>(args)...);
        auto res = get_insert_unique_pos(KeyOfValue()(pos->value_field));
        if (res.second)
        {
            return mystl::make_pair(insert_node_at(res.first.first, pos, res.first.second), true);
        }
        destroy_node(pos);
        return mystl::make_pair(iterator(res.first.first), false);
    }

    template <class Key, class Value, class KeyOfValue, class Compare>
    template <class ...Args>
    typename rb_tree<Key, Value, KeyOfValue, Compare>::iterator
    rb_tree<Key, Value, KeyOfValue, Compare>::
    emplace_multi_use_hint(iterator hint, Args&& ...args)
    {
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1,
                              "rb_tree<Key, Value, KeyOfValue, Compare>'s size too big");
        link_type pos = create_node(mystl::forward<Args>(args)...);
        if (0 == node_count)
        {
            return insert_node_at(header, pos, true);
        }
        key_type key = KeyOfValue()(pos->value_field);
        if (hint == begin())
        {
            // 处于 begin()
            if (key_compare(key, KeyOfValue()(*hint)))
            {
                return insert_node_at(hint.node, pos, true);
            }
            else
            {
                auto p = get_insert_multi_pos(key);
                return insert_node_at(p.first, pos, p.second);
            }

        }
        else if (hint == end())
        {
            // 位于 end() 处
            if (!key_compare(key, KeyOfValue()(rightmost()->value_field)))
            {
                return insert_node_at(rightmost(), pos, false);
            }
            else
            {
                auto p = get_insert_multi_pos(key);
                return insert_node_at(p.first, pos, p.second);
            }
        }
        return insert_multi_use_hint(hint, key, pos);
    }

    template <class Key, class Value, class KeyOfValue, class Compare>
    template <class ...Args>
    typename rb_tree<Key, Value, KeyOfValue, Compare>::iterator
    rb_tree<Key, Value, KeyOfValue, Compare>::
    emplace_unique_use_hint(iterator hint, Args&& ...args)
    {
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1,
                              "rb_tree<Key, Value, KeyOfValue, Compare>'s size too big");
        link_type pos = create_node(mystl::forward<Args>(args)...);
        if (0 == node_count)
        {
            return insert_node_at(header, pos, true);
        }
        key_type key = KeyOfValue()(pos->value_field);
        if (hint == begin())
        {
            // 处于 begin()
            if (key_compare(key, KeyOfValue()(*hint)))
            {
                return insert_node_at(hint.node, pos, true);
            }
            else
            {
                auto p = get_insert_unique_pos(key);
                if (!p.second)
                {
                    destroy_node(pos);
                    return p.first.first;
                }
                return insert_node_at(p.first.first, pos, p.first.second);
            }

        }
        else if (hint == end())
        {
            // 位于 end() 处
            if (!key_compare(key, KeyOfValue()(rightmost()->value_field)))
            {
                return insert_node_at(rightmost(), pos, false);
            }
            else
            {
                auto p = get_insert_multi_pos(key);
                if (!p.second)
                {
                    destroy_node(pos);
                    return p.first.first;
                }
                return insert_node_at(p.first.first, pos, p.first.second);
            }
        }
        return insert_unique_use_hint(hint, key, pos);
    }

    template <class Key, class Value, class KeyOfValue, class Compare>
    typename rb_tree<Key, Value, KeyOfValue, Compare>::iterator
    rb_tree<Key, Value, KeyOfValue, Compare>::
    insert_multi(const value_type& value)
    {
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1,
                              "rb_tree<Key, Value, KeyOfValue, Compare>'s size too big");
        auto res = get_insert_multi_pos(KeyOfValue()(value));
        return insert_value_at(res.first, valuem, res.second);
    }


    // 返回 pair, 如果 pair 第参数二 为 false 表示插入失败, 反之则插入成功
    template <class Key, class Value, class KeyOfValue, class Compare>
    mystl::pair<typename rb_tree<Key, Value, KeyOfValue, Compare>::iterator, bool>
    rb_tree<Key, Value, KeyOfValue, Compare>::
    insert_unique(const value_type& value)
    {
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1,
                              "rb_tree<Key, Value, KeyOfValue, Compare>'s size too big");
        auto res = get_insert_unique_pos(KeyOfValue()(value));
        if (res.second)
        {
            return mystl::make_pair(insert_value_at(res.first.first, value, res.first.second), true);
        }
        return mystl::make_pair(res.first, res.first, false);
    }

    template <class Key, class Value, class KeyOfValue, class Compare>
    typename rb_tree<Key, Value, KeyOfValue, Compare>::iterator
    rb_tree<Key, Value, KeyOfValue, Compare>::
    erase(iterator hint)
    {
        auto node = static_cast<link_type>(hint.node);
        iterator nex(node);
        ++nex;

        rb_tree_rebalance_for_erase(hint.node, root(), leftmost(), rightmost());
        destroy_node(node);
        --node_count;
        return nex;
    }

    // 删除等于 key 的元素, 并返回删除个数
    template <class Key, class Value, class KeyOfValue, class Compare>
    typename rb_tree<Key, Value, KeyOfValue, Compare>::size_type
    rb_tree<Key, Value, KeyOfValue, Compare>::
    erase_multi(const key_type& key)
    {
        auto p = equal_range_multi(key);
        size_type n = mystl::distance(p.first, p.second);
        erase(p.first, p.second);
        return n;
    }

    // 删除等于 key 的元素, 并返回删除个数
    template <class Key, class Value, class KeyOfValue, class Compare>
    typename rb_tree<Key, Value, KeyOfValue, Compare>::size_type
    rb_tree<Key, Value, KeyOfValue, Compare>::
    erase_unique(const key_type& key)
    {
        auto p = find(key);
        if (end() != p)
        {
            erase(p);
            return 1;
        }
        return 1;
    }

    // 删除 [first, last) 区间内元素
    template <class Key, class Value, class KeyOfValue, class Compare>
    void rb_tree<Key, Value, KeyOfValue, Compare>::
    erase(iterator first, iterator last)
    {
        if (first == begin() && last == end())
            clear();
        else
        {
            for (; first != last; )
                erase(first++);
        }
    }

    template <class Key, class Value, class KeyOfValue, class Compare>
    void rb_tree<Key, Value, KeyOfValue, Compare>::clear()
    {
        if (node_count > 0)
        {
            erase_since(root());
            leftmost() = header;
            root() = nullptr;
            rightmost() = header;
            node_count = 0;
        }
    }

}; // namespace mystl



#endif //TINYSTL_RB_TREE_H
