/// @file exception.t.cpp
/// @author Anju Kambadur

#include <AMD/exception.hpp>

#define BOOST_TEST_MODULE ExceptionTest
#include <boost/test/unit_test.hpp>

void foo (void)
{
    // This is the first function that throws the error
    throw AMD::ExceptionImpl(
        APPEND_LOCATION("from foo()"), // location or trace
        "I throw because I can", // error string
        AMD_INVALID_OPERATION); // error code
}

/// Always enclose your functions in a AMD_START_TRY_BLOCK and 
/// AMD_END_TRY_BLOCK construct. No indentation is necessary for
/// these blocks as can be seen.
void bar (void) 
{
    AMD_START_TRY_BLOCK()

    foo();

    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_RETHROW(BAR,foo)
}

/// baz() calls bar(), which calls foo(). The exception thrown in 
/// foo() must be transported all the way to the users.
void baz (void) 
{
    AMD_START_TRY_BLOCK()

    bar();

    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_RETHROW(BAZ,baz)
}

BOOST_AUTO_TEST_CASE(RecursionTest)
{
    AMD_START_TRY_BLOCK()
    baz();
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()
}

