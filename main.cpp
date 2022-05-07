#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#if defined(_MSC_VER) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC 
#include <stdlib.h>
#include <crtdbg.h>
#endif // check memory leaks

#include "Test/list_test.h"
#include "Test/slist_test.h"
#include "Test/stack_test.h"
#include "Test/vector_test.h"
#include "Test/queue_test.h"
#include "Test/set_test.h"
#include "Test/segtree_test.h"
#include "Test/map_test.h"
#include "Test/unordered_set_test.h"
#include "Test/unordered_map_test.h"

int main()
{
    using namespace mystl::test;

    std::cout.sync_with_stdio(false);

    RUN_ALL_TESTS();
    mystl::test::slist_test::slist_test();
    mystl::test::queue_test::queue_test();
#if defined(_MSC_VER) && defined(_DEBUG)
    _CrtDumpMemoryLeaks();
#endif // check memory leaks

}
