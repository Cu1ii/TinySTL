//
// Created by Cu1 on 2022/3/7.
//

#ifndef TINYSTL_ITERATOR_H
#define TINYSTL_ITERATOR_H

#include <cstddef>
#include "type_traits.h"

namespace tstl
{
    // 5 种迭代器的类型
    // (输入迭代器) 迭代器所指的内容不能被修改, 只读且只能执行一次读操作.
    struct input_iterator_tag {};
    // (输出迭代器) : 只写并且一次只能执行一次写操作.
    struct output_iterator_tag {};
    // (正向迭代器) : 支持读写操作且支持多次读写操作.
    struct forward_iterator_tag : public input_iterator_tag{};
    // (双向迭代器) : 支持双向的移动且支持多次读写操作.
    struct bidirectional_iterator_tag : public forward_iterator_tag {};
    // (随即访问迭代器) : 支持双向移动且支持多次读写操作. p+n, p-n等.
    struct random_access_iterator_tag : public bidirectional_iterator_tag {};

    // iterator 模板
    template <class Category, class T, class Distance = ptrdiff_t,
            class Pointer = T*, class Reference = T&>
                    struct iterator
                    {
                        typedef Category                             iterator_category;
                        typedef T                                    value_type;
                        typedef Pointer                              pointer;
                        typedef Reference                            reference;
                        typedef Distance                             difference_type;
                    };

    // iterator traits;

    template <class T>
        struct has_iterator_cat
        {
        private:
            struct two {bool a; bool b;};
            template<class U> static two test(...);

            template<class U> static bool test(typename U::iterator_category* = 0);

        public:
            static const bool value = sizeof (test<T>(0)) == sizeof (bool);
        };

    template <class Iterator, bool>
        struct iterator_traits_impl {};

    template <class Iterator>
        struct iterator_traits_impl<Iterator, true>
        {
            typedef typename Iterator::iterator_category iterator_category;
            typedef typename Iterator::value_type        value_type;
            typedef typename Iterator::pointer           pointer;
            typedef typename Iterator::reference         reference;
            typedef typename Iterator::different_type    different_type;
        };

    template <class Iterator, bool>
        struct iterator_traits_helper {};

    template <class Iterator>
        struct iterator_traits_helper<Iterator, true>
                : public iterator_traits_impl<Iterator,
                std::is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
                std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value>
        {
        };

    // 萃取出迭代器内部定义
    template <class Iterator>
        struct iterator_traits
                :public iterator_traits_helper<Iterator, has_iterator_cat<Iterator>::value> {};

    // 对原生指针进行萃取
    template <class T>
        struct iterator_traits<T*>
        {
            typedef random_access_iterator_tag      iterator_category;
            typedef T                               value_type;
            typedef T*                              pointer;
            typedef T&                              reference;
            typedef ptrdiff_t                       difference_type;
        };

    template <class T>
    struct iterator_traits<const T*>
    {
        typedef random_access_iterator_tag      iterator_category;
        typedef T                               value_type;
        typedef const T*                        pointer;
        typedef const T&                        reference;
        typedef ptrdiff_t                       difference_type;
    };

    template <class T, class U, bool = has_iterator_cat<iterator_traits<T>>::value>
            struct has_iterator_cat_of
                    : public m_bool_constant<std::is_convertible<
                            typename iterator_traits<T>::iterator_category, U>::value>
            {
            };

    // 对迭代器进行萃取
    template <class T, class U>
    struct has_iterator_cat_of<T, U, false> : public m_false_type {};

    template <class Iter>
    struct is_input_iterator : public has_iterator_cat_of<Iter, input_iterator_tag> {};

    template <class Iter>
    struct is_output_iterator : public has_iterator_cat_of<Iter, output_iterator_tag> {};

    template <class Iter>
    struct is_forward_iterator : public has_iterator_cat_of<Iter, forward_iterator_tag> {};

    template <class Iter>
    struct is_bidirectional_iterator : public has_iterator_cat_of<Iter, bidirectional_iterator_tag> {};

    template <class Iter>
    struct is_random_access_iterator : public has_iterator_cat_of<Iter, random_access_iterator_tag> {};

    template <class Iterator>
    struct is_iterator :
            public m_bool_constant<is_input_iterator<Iterator>::value ||
                is_output_iterator<Iterator>::value>
    {
    };

    // 萃取某个迭代器的 distance_type 类型
    template <class Iterator>
    typename iterator_traits<Iterator>::difference_type*
    distance_type(const Iterator&)
    {
        return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
    }

    //萃取某个迭代器的 value_type
    template <class Iterator>
    typename iterator_traits<Iterator>::value_type*
    value_type(const Iterator&)
    {
        return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
    }

    //------------------计算两迭代器之间的距离-----------------------------------

    // 对于 input_iterator_tag 的计算 distance 版本
    template <class InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag)
    {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while (first != last)
        {
            ++first;
            ++n;
        }
        return n;
    }

    // 对于 random_access_iterator_tag 计算 distance 版本
    template <class RandomIterator>
    typename iterator_traits<RandomIterator>::difference_type
    distance_dispatch(RandomIterator first, RandomIterator last,
             random_access_iterator_tag)
    {
        return last - first;
    }

    template <class InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    distance_dispatch(InputIterator first, InputIterator last)
    {
        return distance_dispatch(first, last, iterator_category(first));
    }

    //-------------------让让迭代器前进 n 个距离----------------------------------------

    // input_iterator_tag 的 advance 实现
    template <class InputIterator, class Distance>
    void advance_dispatch(InputIterator& i, Distance n, input_iterator_tag)
    {
        while (n--)
            ++i;
    }

    // bidirectional_iterator_tag 的 advance 实现
    template <class BidirectionalIterator, class Distance>
    void advance_dispatch(BidirectionalIterator &i, Distance n, bidirectional_iterator_tag)
    {
        if (n >= 0)
            while (n--) ++i;
        else
            while (n++) --i;
    }


    // random_access_iterator_tag 的 advance 实现
    template <class RandomIterator, class Distance>
    void advance_dispatch(RandomIterator& i, Distance n, random_access_iterator_tag)
    {
        i += n;
    }

    //根据第三个参数选择性调用上面三个函数 advance
    template <class InputIterator, class Distance>
    void advance(InputIterator& i, Distance n)
    {
        advance_dispatch(i, n, iterator_category(i));
    }

    //=================================反向迭代器==========================================
    // reverse_iterator 前进即后退, 后退即前进
    template <class Iterator>
    class reverse_iterator
    {
    private:
        Iterator current;

    public:
        // 反向迭代器的相应型别
        typedef typename iterator_traits<Iterator>::iterator_category   iterator_category;
        typedef typename iterator_traits<Iterator>::value_type          value_type;
        typedef typename iterator_traits<Iterator>::difference_type     difference_type;
        typedef typename iterator_traits<Iterator>::pointer             pointer;
        typedef typename iterator_traits<Iterator>::reference           reference;

        typedef Iterator                                                iterator_type;
        typedef reverse_iterator<Iterator>                              self;

    public:
        // 构造函数
        reverse_iterator() = default;
        explicit reverse_iterator(iterator_type iterator)
            : current(iterator) {}
        reverse_iterator(const self& rhs)
            : current(rhs.current) {}

    public:
        // 获取 reverse_iterator 中的 iterator
        iterator_type base() const
        { return current;}

        // 重载操作符
        reference operator*() const
        {
            // 获取其正向迭代器中的前一个位置
            iterator_type tmp = current;
            return *(--tmp);
        }

        pointer operator->() const
        {
            return &(operator*());
        }

        // 前进(++) 为 后退(--)
        self& operator++()
        {
            --current;
            return *this;
        }

        self operator++(int)
        {
            self tmp = current;
            --current;
            return tmp;
        }

        // 后退(--) 为 前进(++)
        self& operator--()
        {
            ++current;
            return current;
        }

        self operator--(int)
        {
            self tmp = current;
            ++current;
            return tmp;
        }

        self& operator+=(difference_type n)
        {
            current -= n;
            return *this;
        }

        self& operator-=(difference_type n)
        {
            current += n;
            return *this;
        }

        self operator-(difference_type n) const
        {
            return self(current + n);
        }

        self operator+(difference_type n) const
        {
            return self(current - n);
        }
    };

    // 全局重载
    template <class Iterator>
    typename reverse_iterator<Iterator>::difference_type
    operator-(const reverse_iterator<Iterator>& lhs,
              const reverse_iterator<Iterator>& rhs)
    {
        return rhs.base() - lhs.base();
    }

    template <class Iterator>
    bool operator==(const reverse_iterator<Iterator>& lhs,
            const reverse_iterator<Iterator>& rhs)
    {
        return lhs.base() == rhs.base();
    }

    template <class Iterator>
    bool operator!=(const reverse_iterator<Iterator>& lhs,
                    const reverse_iterator<Iterator>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class Iterator>
    bool operator<(const reverse_iterator<Iterator>& lhs,
            const reverse_iterator<Iterator>& rhs)
    {
        return lhs.base() < rhs.base();
    }

    template <class Iterator>
    bool operator>(const reverse_iterator<Iterator>& lhs,
                   const reverse_iterator<Iterator>& rhs)
    {
        return rhs < lhs;
    }

    template <class Iterator>
    bool operator<=(const reverse_iterator<Iterator>& lhs,
            const reverse_iterator<Iterator>& rhs)
    {
        return !(lhs > rhs);
    }

    template <class Iterator>
    bool operator>=(const reverse_iterator<Iterator>& lhs,
            const reverse_iterator<Iterator>& rhs)
    {
        return !(lhs < rhs);
    }
}


#endif //TINYSTL_ITERATOR_H
