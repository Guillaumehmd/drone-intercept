#include <gtest/gtest.h>

#include "di/version.hpp"

TEST(Smoke, VersionIsNotEmpty) {
    EXPECT_FALSE(di::version().empty());
}
