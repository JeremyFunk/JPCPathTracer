#include "gtest/gtest.h"
#include "utilities/arithmetic.h"
#include "core/Linalg.h"
namespace jpc_tracer {

TEST(utilities, solve_quadratic)
{
    Prec a = 0;
    Prec b = 2;
    Prec c = 1;

    Prec out1;
    Prec out2;

    bool return_value = solve_quadratic(a, b, c, &out1, &out2);

    EXPECT_EQ(return_value, true);

    EXPECT_EQ(out2, -0.5);

    //std::cout << out1 << ' ' << out2 << '\n';
}
}