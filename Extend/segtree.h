//
// Created by Cu1 on 2022/4/6.
//

#ifndef TINYSTL_SEGTREE_H
#define TINYSTL_SEGTREE_H

// 参照 atcoder 并使用了提供的 internal_bit.hpp
// 包含一个模板类 segtree (线段树) 以 mystl::vector 作为底层容器


#include "internal_bit.hpp"
#include "../TinySTL/vector.h"

namespace extend
{
    using namespace mystl;

    /**
     * 共有 3 个模板参数
     * @tparam S 表示线段树的存储类型
     * @tparam op 对两个数据类型进行线段树合并的操作, 且必须符合区间加法, 不符合的则不保证答案正确性
     * @tparam e 返回一个提供初始化元素 S 的函数 e()
     */
    template <class S, S (*op)(S, S), S (*e)()>
    struct segtree
    {

    public:
        typedef  S  node_type; // 节点要维护的值

        segtree() : segtree(0) {}

        explicit segtree(int n)
                :segtree(vector<node_type>(n, e()))
        {
        }

        explicit segtree(const vector<node_type>& v)
                :_n(int(v.size()))
        {
            // 求 log2(n) 向上取整
            log = internal::ceil_pow2(_n);
            size = (1 << log);
            d = mystl::vector<node_type>(2 * size, e());
            for (int i = 0; i < _n; i++)
                d[size + i] = v[i];
            for (int i = size - 1; i >= 1; i--)
                update(i);
        }

        void set(int p, node_type x)
        {
            p += size;
            d[p] = x;
            for (int i = 1; i <= log; i++)
                update(p >> i);
        }

        node_type get(int p) const
        {
            return d[p + size];
        }

        node_type prod(int l, int r) const
        {
            node_type sml = e(), smr = e();
            l += size, r += size;

            while (l < r)
            {
                if (l & 1)
                    sml = op(sml, d[l++]);
                if (r & 1)
                    smr = op(d[--r], smr);
                l = l >> 1;
                r = r >> 1;
            }
            return op(sml, smr);
        }

        node_type all_prod() const { return d[1]; }

        // 针对函数指针进行的偏特化
        template<bool (*f)(S)>
        int max_right(int l) const
        { return max_right(l, [](S x) { return f(x); } ); }

        // 提供一个函数对象 F, 找到满足 F() 的最右边界
        // 要求 F 单调, 不单调的话不保证, 且保证 F(e()) 为 true
        template<class F>
        int max_right(int l, F f) const
        {

            if (l == _n)
                return _n;
            l = l + size;
            node_type sm = e();
            do {
                while (l % 2 == 0)
                    l = l >> 1;
                if (!f(op(sm, d[l])))
                {
                    while (l < size)
                    {
                        l = l * 2;
                        if (f(op(sm, d[l])))
                        {
                            sm = op(sm, d[l]);
                            l++;
                        }
                    }
                    return l - size;
                }
                sm = op(sm, d[l]);
                l++;
            } while ((l & -l) != l) ;
            return _n;
        }

        // 针对函数指针进行的偏特化
        template<bool (*f)(S)>
        int min_left(int r) const
        { return min_left(r, [](S x) { return f(x); } ); }

        template <class F>
        int min_left(int r, F f) const
        {
            if (r == 0) return _n;
            r = r + size;
            node_type sm = e();
            do {
                r--;
                while (r > 1 && (r % 2))
                    r = r >> 1;
                if (!f(op(d[r], sm)))
                {
                    while (r < size)
                    {
                        r = (2 * r + 1);
                        if (f(op(d[r], sm)))
                        {
                            sm = op(d[r], sm);
                            r--;
                        }
                    }
                    return r + 1 - size;
                }
                sm = op(d[r], sm);
            } while ((r & -r) != r);
            return 0;
        }


    private:
        int left(int k)     const { return 2 * k; }
        int right(int k)    const { return 2 * k + 1; }
        void update(int k) { d[k] = op(d[left(k)], d[right(k)]); }

        vector<node_type> d;
        int _n, size, log;

    };
}

#endif //TINYSTL_SEGTREE_H
