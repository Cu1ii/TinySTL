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
        typedef rb_tree_color_type                  color_type;
        typedef rb_tree_node_base*                  base_ptr;
        typedef rb_tree_node<Value>                 rb_tree_node;


        typedef mystl::allocator<T>                 allocator_type;
        typedef mystl::allocator<T>                 data_allocator;
        typedef mystl::allocator<rb_tree_node_base> base_allocator;
        typedef mystl::allocator<rb_tree_node>      node_allocator;

    public:
        typedef Key                                 key_type;
        typedef Value                               value_type;
        typedef value_type*                         pointer;
        typedef const value_type*                   const_pointer;
        typedef value_type&                         reference;
        typedef const value_type&                   const_reference;
        typedef rb_tree_node*                       link_type;
        typedef size_t                              size_type;
        typedef ptrdiff_t                           difference_type;
    };

}; // namespace mystl



#endif //TINYSTL_RB_TREE_H
