#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#if defined(_MSC_VER) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC 
#include <stdlib.h>
#include <crtdbg.h>
#endif // check memory leaks

#include "Test/list_test.h"
#include "Test/stack_test.h"
#include "Test/vector_test.h"

int main()
{
    using namespace mystl::test;

    std::cout.sync_with_stdio(false);

    RUN_ALL_TESTS();

    list_test::list_test();
    stack_test::stack_test();
    vector_test::vector_test();

#if defined(_MSC_VER) && defined(_DEBUG)
    _CrtDumpMemoryLeaks();
#endif // check memory leaks

}
