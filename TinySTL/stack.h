//
// Created by Cu1 on 2022/3/21.
//

#ifndef TINYSTL_STACK_H
#define TINYSTL_STACK_H

#include "list.h"

namespace mystl
{

    template <class T, class Container = mystl::list<T>>
    class stack
    {
    public:
        typedef typename Container::value_type      value_type;
        typedef typename Container::size_type       size_type;
        typedef typename Container::reference       reference;
        typedef typename Container::const_reference const_reference;

    protected:
        Container c;

    public:

        stack() = default;

        explicit stack(size_type n)
            : c(n) {}

        stack(size_type n, const value_type& value)
            : c(n, value)
        {
        }

        template <class Iter>
        stack(Iter first, Iter last)
            : c(first, last)
        {
        }

        stack(std::initializer_list<T> ilist)
            : c(ilist.begin(), ilist.end())
        {
        }

        stack(const Container& c_)
            : c(c_)
        {
        }

        stack(Container&& c_) noexcept(std::is_nothrow_constructible<Container>::value)
            : c(mystl::move(c_))
        {
        }

        stack(const stack& rhs)
            : c(rhs.c)
        {
        }

        stack(stack&& rhs) noexcept(std::is_nothrow_constructible<Container>::value)
            : c(mystl::move(rhs.c))
        {
        }

        stack& operator=(const stack& rhs)
        {
            c = rhs.c;
            return *this;
        }

        stack& operator=(const stack&& rhs)
        {
            c = mystl::move(rhs.c);
            return *this;
        }

        stack& operator=(std::initializer_list<T> ilist)
        {
            c = ilist;
            return *this;
        }

        bool empty() const { return c.empty(); }
        size_type size() const { return c.size(); }
        reference top() { return c.back(); }
        const_reference top() const { return c.back(); }
        void push(const value_type& value) { c.push_back(value); }
        void pop() { c.pop_back(); }
        void swap(stack& rhs) noexcept(noexcept(mystl::swap(c, rhs.c)))
        {
            mystl::swap(c, rhs.c);
        }
        void clear() { c.clear(); }

        bool operator==(const stack<T, Container>& x)
        {
            return c == x.c;
        }

        bool operator<(const stack<T, Container>& x)
        {
            return c < x.c;
        }


    };

} // namespace mystl


#endif //TINYSTL_STACK_H
