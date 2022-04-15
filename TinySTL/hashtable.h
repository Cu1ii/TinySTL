//
// Created by Cu1 on 2022/4/14.
//

#ifndef TINYSTL_HASHTABLE_H
#define TINYSTL_HASHTABLE_H

// 这个头文件包含了一个模板类 hashtable
// hashtable: 哈希表, 使用拉链法处理冲突

#include <initializer_list>

#include "algo.h"
#include "functional.h"
#include "memory.h"
#include "vector.h"
#include "util.h"
#include "exceptdef.h"

namespace mystl
{
    // hashtable 节点定义
    template <T>
    struct hashtable_node
    {
        hashtable_node* next;
        T               value;

        hashtable_node() = default;
        hashtable_node(const T& n) :next(nullptr), value(n)
        {
        }

        hashtable_node(const hashtable_node<T>& node)
            : next(node.next), value(node.value)
        {
        }

        hashtable_node(hashtable_node&& node)
            : next(node.next), value(mystl::move(node.value))
        {
            node.next = nullptr;
        }
    };

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
    class hashtable;

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
    struct hashtable_iterator;

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
    struct hashtable_const_iterator;

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
    struct hashtable_iterator
    {
        typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>                  hashtable;
        typedef hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>         iterator;
        typedef hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>   const_iterator;
        typedef hashtable_node<Value>                                                 node;

        typedef forward_iterator_tag                                                  iterator_tag;
        typedef Value                                                                 value_type;
        typedef ptrdiff_t                                                             difference_type;
        typedef size_t                                                                size_type;
        typedef Value&                                                                reference;
        typedef Value*                                                                pointer;

        node* cur;
        hashtable* ht;

        hashtable_iterator() = default;
        hashtable_iterator(node* n, hashtable* tab)
            : cur(n), ht(tab)
        {
        }

        hashtable_iterator(const iterator& other)
            : cur(other.cur), ht(other.ht)
        {
        }

        hashtable_iterator(const const_iterator& other)
            : cur(other.cur), ht(other.ht)
        {
        }

        iterator& operator=(const iterator& rhs)
        {
            if (this != &rhs)
            {
                cur = rhs.cur;
                ht  = rhs.ht;
            }
            return *this;
        }

        iterator& operator=(const const_iterator& rhs)
        {
            if (this != &rhs)
            {
                cur = rhs.cur;
                ht  = rhs.ht;
            }
        }

        reference operator*()   const { return cur->value; }
        pointer   operator->()  const { return &(operator*()); }

        iterator& operator++();
        iterator& operator++(int);

        bool operator==(const iterator& other) const { return cur == it.cur};
        bool operator!=(const iterator& other) const { return cur != it.cur};
    };

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
    struct hashtable_const_iterator
    {
        typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>                  hashtable;
        typedef hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>         iterator;
        typedef hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>   const_iterator;
        typedef hashtable_node<Value>                                                 node;

        typedef forward_iterator_tag                                                  iterator_tag;
        typedef Value                                                                 value_type;
        typedef ptrdiff_t                                                             difference_type;
        typedef size_t                                                                size_type;
        typedef const Value&                                                          reference;
        typedef const Value*                                                          pointer;

        const node* cur;
        const hashtable* ht;

        hashtable_const_iterator() = default;
        hashtable_const_iterator(const node* n, const hashtable* tab)
            : cur(n), ht(tab)
        {
        }

        hashtable_const_iterator(const iterator& other)
            : cur(other.cur), ht(other.ht)
        {
        }

        hashtable_const_iterator(const const_iterator& other)
            : cur(other.cur), ht(other.ht)
        {
        }

        const_iterator& operator=(const const_iterator& rhs)
        {
            if (this != &rhs)
            {
                cur = rhs.cur;
                ht  = rhs.ht;
            }
        }

        const_iterator& operator=(const iterator& rhs)
        {
            if (this != &rhs)
            {
                cur = rhs.cur;
                ht  = rhs.ht;
            }
        }

        reference operator*()   const { return cur->value; }
        pointer   operator->()  const { return &(operator*()); }

        const_iterator& operator++();
        const_iterator& operator++(int);

        bool operator==(const const_iterator& rhs) const { return cur == rhs.cur; }
        bool operator!=(const const_iterator& rhs) const { return cur != rhs.cur; }
    };


    // bucket 大小
#if (_MSC_VER && WIN64) || ((__GNUC__ || __clang__) && __SIZEOF_POINTER__ == 8)
#define SYSTEM_64 1
#else
#define SYSTEM_32 1
#endif

#ifdef SYSTEM_64
#define PRIME_NUM 99
    // 1. start with p = 101
    // 2. p = next_prime(p * 1.7)
    // 3. if p < (2 << 63), go to step 2, otherwise, go to step 4
    // 4. end with p = prev_prime(2 << 63 - 1)
    static constexpr size_t stl_prime_list[] =
    {
        101ull, 173ull, 263ull, 397ull, 599ull, 907ull, 1361ull, 2053ull, 3083ull,
        4637ull, 6959ull, 10453ull, 15683ull, 23531ull, 35311ull, 52967ull, 79451ull,
        119179ull, 178781ull, 268189ull, 402299ull, 603457ull, 905189ull, 1357787ull,
        2036687ull, 3055043ull, 4582577ull, 6873871ull, 10310819ull, 15466229ull,
        23199347ull, 34799021ull, 52198537ull, 78297827ull, 117446801ull, 176170229ull,
        264255353ull, 396383041ull, 594574583ull, 891861923ull, 1337792887ull,
        2006689337ull, 3010034021ull, 4515051137ull, 6772576709ull, 10158865069ull,
        15238297621ull, 22857446471ull, 34286169707ull, 51429254599ull, 77143881917ull,
        115715822899ull, 173573734363ull, 260360601547ull, 390540902329ull,
        585811353559ull, 878717030339ull, 1318075545511ull, 1977113318311ull,
        2965669977497ull, 4448504966249ull, 6672757449409ull, 10009136174239ull,
        15013704261371ull, 22520556392057ull, 33780834588157ull, 50671251882247ull,
        76006877823377ull, 114010316735089ull, 171015475102649ull, 256523212653977ull,
        384784818980971ull, 577177228471507ull, 865765842707309ull, 1298648764060979ull,
        1947973146091477ull, 2921959719137273ull, 4382939578705967ull, 6574409368058969ull,
        9861614052088471ull, 14792421078132871ull, 22188631617199337ull, 33282947425799017ull,
        49924421138698549ull, 74886631708047827ull, 112329947562071807ull, 168494921343107851ull,
        252742382014661767ull, 379113573021992729ull, 568670359532989111ull, 853005539299483657ull,
        1279508308949225477ull, 1919262463423838231ull, 2878893695135757317ull, 4318340542703636011ull,
        6477510814055453699ull, 9716266221083181299ull, 14574399331624771603ull, 18446744073709551557ull
    };

#else
#define PRIME_NUM 44
    // 1. start with p = 101
    // 2. p = next_prime(p * 1.7)
    // 3. if p < (2 << 31), go to step 2, otherwise, go to step 4
    // 4. end with p = prev_prime(2 << 31 - 1)
    static constexpr size_t stl_prime_list[] =
    {
        101u, 173u, 263u, 397u, 599u, 907u, 1361u, 2053u, 3083u, 4637u, 6959u,
        10453u, 15683u, 23531u, 35311u, 52967u, 79451u, 119179u, 178781u, 268189u,
        402299u, 603457u, 905189u, 1357787u, 2036687u, 3055043u, 4582577u, 6873871u,
        10310819u, 15466229u, 23199347u, 34799021u, 52198537u, 78297827u, 117446801u,
        176170229u, 264255353u, 396383041u, 594574583u, 891861923u, 1337792887u,
        2006689337u, 3010034021u, 4294967291u,
    };
#endif

    // 找到 >= n 的第一个指数
    inline size_t stl_next_prime(size_t n)
    {
        const size_t* first = stl_prime_list;
        const size_t* last  = stl_prime_list + PRIME_NUM;
        const size_t* pos   = mystl::lower_bound(first, last, n);
        return pos == last ? *(last - 1) : *pos;
    }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
    class hashtable
    {
    public:
        typedef Key                                                 key_type;
        typedef Value                                               value_type;
        typedef HashFcn                                             hasher;
        typedef EqualKey                                            key_equal;

        typedef hashtable_node<Value>   node;
        typedef mystl::allocator<Value>                             allocator_type;
        typedef mystl::allocator<Value>                             data_allocator;
        typedef mystl::allocator<node>                              node_allocator;

        typedef typename allocator_type::pointer                    pointer;
        typedef typename allocator_type::const_pointer              const_pointer;
        typedef typename allocator_type::reference                  reference;
        typedef typename allocator_type::const_reference            const_reference;
        typedef typename allocator_type::size_type                  size_type;
        typedef typename allocator_type::difference_type            difference_type;

        typedef mystl::hashtable_iterator<Value, Key, HashFcn,
                                            ExtractKey, EqualKey>   iterator;
        typedef mystl::hashtable_const_iterator<Value, Key, HashFcn,
                                            ExtractKey, EqualKey>   const_iterator;

        friend struct
                mystl::hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>;
        friend struct
                mystl::hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>;

        hasher      hash_funct()        const { return hash; }
        key_equal   key_eq()            const { return equals; }
        allocator_type get_allocator()  const { return allocator_type(); }
    private:

        typedef mystl::vector<node*>    bucket_type;

        bucket_type buckets;
        size_type   num_elements;
        hasher      hash;
        key_equal   equals;
        ExtractKey  get_key;

    public:

        explicit hashtable(size_type bucket_count,
                           const HashFcn& hf = HashFcn(),
                           const EqualKey& eql = EqualKey())
            : hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0)
        {
            // TODO
            initialize_buckets(n);
        }

        template <class Iter, typename std::enable_if<
                mystl::is_input_iterator<Iter>::value, int>::type = 0>
        hashtable(Iter first, Iter last,
                  size_type bucket_count,
                  const HashFcn& hf = HashFcn(),
                  const EqualKey& eql = EqualKey())
            : get_key(ExtractKey), hash(hf), equals(eql), num_elements(mystl::distance(first, last))
        {
            // TODO
            initialize_buckets(mystl::max(bucket_count, static_cast<size_type>(mystl::distance(first, last))));
        }

        hashtable(const hashtable& other)
            : hash(other.hash), equals(other.equals), get_key(ExtractKey()), num_elements(0)
        {
            copy_from(other);
        }

        hashtable(hashtable&& other) noexcept
            : get_key(other.get_key),
              hash(other.hash),
              equals(other.equals),
              num_elements(other.num_elements)
        {
            buckets = mystl::move(other.buckets);
            other.num_elements = 0;
        }

        hashtable& operator=(const hashtable& rhs)
        {
            if (this != &rhs)
            {
                // TODO
                clear();
                hash = rhs.hash;
                equals = rhs.equals;
                get_key = rhs.get_key;
                //TODO
                copy_from(rhs);
            }
            return *this;
        }

        hashtable& operator=(hashtable&& rhs) noexcept
        {
            if (this != &rhs)
            {
                // TODO
                clear();
                hash = rhs.hash;
                equals = rhs.equals;
                get_key = rhs.get_key;
                num_elements = rhs.num_elements;
                buckets = mystl::move(rhs.buckets);
                rhs.num_elements = 0;
            }
        }

        // TOOD
        ~hashtable() { clear(); }

    public:

        iterator begin() noexcept
        {
            for (size_type n = 0; n < buckets.size(); ++n)
                if (buckets[n])
                    return iterator(buckets[n], this);
            return end();
        }
        const_iterator begin() const noexcept
        {
            for (size_type n = 0; n < buckets.size(); ++n)
                if (buckets[n])
                    return const_iterator(buckets[n], this);
            return end();
        }

        iterator end()                noexcept { return iterator(0, this); }
        const_iterator end()    const noexcept { return const_iterator(0, this); }

        const_iterator cbegin() const noexcept { return begin(); }
        const_iterator cend()   const noexcept { return end(); }

    };


};




#endif //TINYSTL_HASHTABLE_H
