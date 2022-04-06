//
// Created by Cu1 on 2022/4/6.
//

#ifndef TINYSTL_SEGTREE_TEST_H
#define TINYSTL_SEGTREE_TEST_H

#include "../Extend/segtree.h"
#include "test.h"

namespace mystl
{

namespace test
{

namespace segtree_test
{
    int merge(int a, int b) { return a + b; };
    int e() { return 0; };

    void segtree_print(extend::segtree<int, merge, e>& s)
    {

    }

// segtree 的遍历输出
#define SEGTREE_COUT(s) do {                 \
    std::string s_name = #s;                 \
    std::cout << " " << s_name << ": ";      \
    segtree_print(s);                        \
} whike (0)


} // namespace segtree_test

}// namespace test

} // namespace stack_test

#endif //TINYSTL_SEGTREE_TEST_H
