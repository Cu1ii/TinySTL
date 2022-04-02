//
// Created by Cu1 on 2022/3/31.
//

#ifndef TINYSTL_QUEUE_H
#define TINYSTL_QUEUE_H

// 包含了 queue 以及 priority_queue
// queue 队列
// priority_queue 优先队列
#include "list.h"
#include "vector.h"
#include "functional.h"
#include "heap_algo.h"

namespace mystl
{
    // 模板类 queue
    // 第一个参数代表数据类型, 第二个参数表示适配的底层容器类型, 缺省使用 mystl::list 作为底层容器
    template <class T, class Container = mystl::list<T>>
    class queue
    {
    public:
        typedef Container                               container_type;
        typedef typename Container::value_type          value_type;
        typedef typename Container::size_type           size_type;
        typedef typename Container::reference           reference;
        typedef typename Container::const_reference     const_reference;

        static_assert(std::is_same<T, value_type>::value,
                      "the value_type of Container shoule be same with T");
    private:
        // 容器底层默认使用 list
        container_type c;

    public:
        queue() = default;
        explicit queue(size_type n)
            : c(n)
        {
        }

        queue(size_type n, const value_type value)
            : c(n, value)
        {
        }

        template<class Iter>
        queue(Iter first, Iter last)
            : c(first, last)
        {
        }

        queue(std::initializer_list<T> ilist)
            : c(ilist.begin(), ilist.end())
        {
        }

        queue(const Container& other)
            : c(other)
        {
        }

        queue(Container&& other)
            : c(mystl::move(other))
        {
        }

        queue(const queue<T, Container>& other)
            : c(other.c)
        {
        }

        queue(queue<T, Container>&& other)
            : c(mystl::move(other.c))
        {
        }

        queue<T, Container>& operator=(const queue<T, Container>& rhs)
        {
            c = rhs.c;
            return *this;
        }

        queue<T, Container>& operator=(queue<T, Container>&& rhs)
        {
            c = mystl::move(rhs.c);
            return *this;
        }

        queue<T, Container>& operator=(std::initializer_list<T> ilist)
        {
            c = ilist;
            return *this;
        }

        ~queue() = default;

        // 访问基础元素
        reference       front()                    { return c.front(); }
        const_reference front()     const          { return c.front(); }
        reference       back()                     { return c.back();  }
        const_reference back()      const          { return c.back();  }
        bool            empty()     const noexcept { return c.empty(); }
        size_type       size()      const noexcept { return c.size();  }


        // 队列操作
        template<class ...Args>
        void emplace(Args&& ...args)
        { c.emplace_back(mystl::forward<Args>(args)...); }

        void push(const value_type& value)
        { c.push_back(value); }
        void push(value_type&& value)
        { c.emplace_back(value); }

        void pop()
        { c.pop_front(); }

        void clear()
        {
            while (!empty()) pop();
        }

        void swap(queue<T, Container>& rhs) noexcept(noexcept(mystl::swap(c, rhs.c)))
        { mystl::swap(c, rhs.c); }

    public:
        bool operator==(const queue<T, Container>& rhs) { return this->c == rhs.c; }
        bool operator<(const queue<T, Container>& rhs)  { return this->c < rhs.c;  }
        bool operator!=(const queue<T, Container>& rhs) { return !(this->c == rhs.c); }
        bool operator>(const queue<T, Container>& rhs)  { return rhs.c < this->c;  }
        bool operator<=(const queue<T, Container>& rhs) { return !(*this > rhs);  }
        bool operator>=(const queue<T, Container>& rhs) { return !(*this < rhs);  }
    };

    template <class T, class Container>
    void swap(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
    {
        lhs.swap(rhs);
    }

    /*****************************************************************************************/
    // 模板类 priority_queue
    // 第一个模板参数表示代表数据类型
    // 第二个模板参数表示底层容器类型(必须是连续型容器), 缺省使用 mystl::vector
    // 第三个模板参数表示元素优先级的判定方式, 缺省使用仿函数 mystl::less 作为默认判定方式

    template <class T, class Sequence = mystl::vector<T>,
              class Compare = mystl::less<T>>
    class priority_queue
    {
    public:
        typedef Sequence    sequence_type;
        typedef Compare     value_compare;

        typedef typename Sequence::value_type       value_type;
        typedef typename Sequence::size_type        size_type;
        typedef typename Sequence::reference        reference;
        typedef typename Sequence::const_reference  const_reference;

        static_assert(std::is_same<T, value_type>::value,
                      "the value_type of Sequence shoule be same with T");

    private:
        sequence_type c;
        value_compare comp;

    public:
        priority_queue() = default;

        priority_queue(const Compare& cmp)
            : c(), comp(cmp)
        {
        }

        explicit priority_queue(size_type n)
            : c(n)
        {
            mystl::make_heap(c.begin(), c.end(), comp);
        }

        priority_queue(size_type n, const value_type& value)
            : c(n, value)
        {
            mystl::make_heap(c.begin(), c.end(), comp);
        }

        template <class Iter>
        priority_queue(Iter first, Iter last)
            : c(first, last)
        {
            mystl::make_heap(c.begin(), c.end(), comp);
        }

        priority_queue(std::initializer_list<T> ilist)
            : c(ilist.begin(), ilist.end())
        {
            mystl::make_heap(c.begin(), c.end());
        }

        priority_queue(const Sequence& s)
            : c(s)
        {
            mystl::make_heap(c.begin(), c.end(), comp);
        }
        priority_queue(Sequence&& s)
            : c(mystl::move(s))
        {
            mystl::make_heap(c.begin(), c.end(), comp);
        }

        priority_queue(const priority_queue<T>& other)
            : c(other.c), comp(other.comp)
        {
            mystl::make_heap(c.begin(), c.end(), comp);
        }

        priority_queue(priority_queue<T>&& other)
            : c(mystl::move(other.c)), comp(other.comp)
        {
            mystl::make_heap(c.begin(), c.end(), comp);
        }

        priority_queue<T>&
                operator=(const priority_queue<T>& rhs)
        {
            c = rhs.c;
            comp = rhs.comp;
            mystl::make_heap(c.begin(), c.end(), comp);
            return *this;
        }

        priority_queue<T>& operator=(priority_queue<T>&& rhs)
        {
            c = mystl::move(rhs.c);
            comp = rhs.comp;
            mystl::make_heap(c.begin(), c.end(), comp);
            return *this;
        }

        priority_queue<T>& operator=(std::initializer_list<T> ilist)
        {
            c = ilist;
            comp = value_compare();
            mystl::make_heap(c.begin(), c.end(), comp);
            return *this;
        }

        ~priority_queue() = default;
        // 访问元素相关操作
        const_reference top()   const          { return c.front(); }

        // 容量相关操作
        bool            empty() const noexcept { return c.empty(); }
        size_type       size()  const noexcept { return c.size(); }

        template <class... Args>
        void emplace(Args&& ...args)
        {
            c.emplace_back(mystl::forward<Args>(args)...);
            mystl::push_heap(c.begin(), c.end(), comp);
        }

        void push(const value_type& value)
        {
            c.push_back(value);
            mystl::push_heap(c.begin(), c.end(), comp);
        }

        void pop()
        {
            mystl::pop_heap(c.begin(), c.end());
            c.pop_back();
        }

        void clear()
        {
            while (!empty()) pop();
        }

        void swap(priority_queue<T>& rhs)  noexcept(noexcept(mystl::swap(c, rhs.c)) &&
                                                    noexcept(mystl::swap(comp, rhs.comp)))
        {
            mystl::swap(c, rhs.c);
            mystl::swap(comp, rhs.comp);
        }

    public:

        bool operator==(const priority_queue<T>& rhs) const
        { return this->c == rhs.c; }

        bool operator!=(const priority_queue<T>& rhs)
        { return this->c < rhs.c; }
    };

    template <class T, class Sequence, class Compare>
    void swap(priority_queue<T>& lhs, priority_queue<T>& rhs)
              noexcept(noexcept(lhs.swap(rhs)))
    {
        lhs.swap(rhs);
    }
}





#endif //TINYSTL_QUEUE_H
