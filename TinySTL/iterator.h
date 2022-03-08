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


}


#endif //TINYSTL_ITERATOR_H
