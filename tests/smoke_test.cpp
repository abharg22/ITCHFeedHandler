// Smoke test: proves the toolchain, vcpkg, GoogleTest and ctest are wired up.
// Real tests start with ITCH-012.

#include <gtest/gtest.h>

// Including every public header keeps them compiling cleanly under
// -Wall -Wextra -Werror as they gain content.
#include <itch/messages.hpp>
#include <itch/order_book.hpp>
#include <itch/parser.hpp>
#include <itch/types.hpp>

TEST(Smoke, BuildIsWiredUp) {
    EXPECT_EQ(1 + 1, 2);
}

TEST(Smoke, CompiledAsCpp20) {
    EXPECT_GE(__cplusplus, 202002L);
}
