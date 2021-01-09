#include "gtest/gtest.h"
#include "utilities/arithmetic.h"
#include "core/Linalg.h"

TEST(utilities, solve_quadratic)
{
    core::Prec a = 0;
    core::Prec b = 2;
    core::Prec c = 1;

    core::Prec out1;
    core::Prec out2;

    bool return_value = utilities::solve_quadratic(a, b, c, &out1, &out2);

    EXPECT_EQ(return_value, true);

    EXPECT_EQ(out2, -0.5);

    //std::cout << out1 << ' ' << out2 << '\n';
}