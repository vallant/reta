#include "util/Permutation.h"

#include <catch2/catch.hpp>

TEST_CASE ("Permutation works", "[unit][util]")
{
    SECTION ("permuts work for size 5+1")
    {
        auto actual = Permutation::generatePermutations (5, 1);
        std::set<std::vector<size_t>> reference {{0}, {1}, {2}, {3}, {4}};
        REQUIRE (actual == reference);
    }
    SECTION ("permuts work for size 2+2")
    {
        auto actual = Permutation::generatePermutations (2, 2);
        std::set<std::vector<size_t>> reference {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
        REQUIRE (actual == reference);
    }

    SECTION ("permuts work for size 3+2")
    {
        auto actual = Permutation::generatePermutations (3, 2);
        std::set<std::vector<size_t>> reference {{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1},
                                                 {1, 2}, {2, 0}, {2, 1}, {2, 2}};
        REQUIRE (actual == reference);
    }

    SECTION ("permuts work for size 4+2")
    {
        auto actual = Permutation::generatePermutations (4, 2);
        std::set<std::vector<size_t>> reference {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 0}, {1, 1}, {1, 2}, {1, 3},
                                                 {2, 0}, {2, 1}, {2, 2}, {2, 3}, {3, 0}, {3, 1}, {3, 2}, {3, 3}};
        REQUIRE (actual == reference);
    }

    SECTION ("permuts work for size 4+3")
    {
        auto actual = Permutation::generatePermutations (4, 3);
        std::set<std::vector<size_t>> reference {
            {0, 0, 0}, {0, 0, 1}, {0, 0, 2}, {0, 0, 3}, {0, 1, 0}, {0, 1, 1}, {0, 1, 2}, {0, 1, 3},
            {0, 2, 0}, {0, 2, 1}, {0, 2, 2}, {0, 2, 3}, {0, 3, 0}, {0, 3, 1}, {0, 3, 2}, {0, 3, 3},
            {1, 0, 0}, {1, 0, 1}, {1, 0, 2}, {1, 0, 3}, {1, 1, 0}, {1, 1, 1}, {1, 1, 2}, {1, 1, 3},
            {1, 2, 0}, {1, 2, 1}, {1, 2, 2}, {1, 2, 3}, {1, 3, 0}, {1, 3, 1}, {1, 3, 2}, {1, 3, 3},
            {2, 0, 0}, {2, 0, 1}, {2, 0, 2}, {2, 0, 3}, {2, 1, 0}, {2, 1, 1}, {2, 1, 2}, {2, 1, 3},
            {2, 2, 0}, {2, 2, 1}, {2, 2, 2}, {2, 2, 3}, {2, 3, 0}, {2, 3, 1}, {2, 3, 2}, {2, 3, 3},
            {3, 0, 0}, {3, 0, 1}, {3, 0, 2}, {3, 0, 3}, {3, 1, 0}, {3, 1, 1}, {3, 1, 2}, {3, 1, 3},
            {3, 2, 0}, {3, 2, 1}, {3, 2, 2}, {3, 2, 3}, {3, 3, 0}, {3, 3, 1}, {3, 3, 2}, {3, 3, 3}};
        REQUIRE (actual == reference);
    }

    SECTION ("permuts work for size 3+3")
    {
        auto actual = Permutation::generatePermutations (3, 3);
        std::set<std::vector<size_t>> reference {
            {0, 0, 0}, {0, 0, 1}, {0, 0, 2}, {0, 1, 0}, {0, 1, 1}, {0, 1, 2}, {0, 2, 0}, {0, 2, 1}, {0, 2, 2},
            {1, 0, 0}, {1, 0, 1}, {1, 0, 2}, {1, 1, 0}, {1, 1, 1}, {1, 1, 2}, {1, 2, 0}, {1, 2, 1}, {1, 2, 2},
            {2, 0, 0}, {2, 0, 1}, {2, 0, 2}, {2, 1, 0}, {2, 1, 1}, {2, 1, 2}, {2, 2, 0}, {2, 2, 1}, {2, 2, 2}};
        REQUIRE (actual == reference);
    }

    SECTION ("permuts work for size 1+1")
    {
        auto actual = Permutation::generatePermutations (1, 1);
        std::set<std::vector<size_t>> reference {{0}};
        REQUIRE (actual == reference);
    }

    SECTION ("permuts work for size 1+2")
    {
        auto actual = Permutation::generatePermutations (1, 2);
        std::set<std::vector<size_t>> reference {{0, 0}};
        REQUIRE (actual == reference);
    }

    SECTION ("permuts works with different sets")
    {
        auto actual = Permutation::generatePermutations ({2, 3});
        std::set<std::vector<size_t>> reference {{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2}};
        REQUIRE (actual == reference);
    }

    SECTION ("permuts works with same sets")
    {
        auto actual = Permutation::generatePermutations ({2, 2});
        std::set<std::vector<size_t>> reference {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
        REQUIRE (actual == reference);
    }

    SECTION ("permuts work for same sets with size 3")
    {
        auto actual = Permutation::generatePermutations ({3, 3, 3});
        std::set<std::vector<size_t>> reference {
            {0, 0, 0}, {0, 0, 1}, {0, 0, 2}, {0, 1, 0}, {0, 1, 1}, {0, 1, 2}, {0, 2, 0}, {0, 2, 1}, {0, 2, 2},
            {1, 0, 0}, {1, 0, 1}, {1, 0, 2}, {1, 1, 0}, {1, 1, 1}, {1, 1, 2}, {1, 2, 0}, {1, 2, 1}, {1, 2, 2},
            {2, 0, 0}, {2, 0, 1}, {2, 0, 2}, {2, 1, 0}, {2, 1, 1}, {2, 1, 2}, {2, 2, 0}, {2, 2, 1}, {2, 2, 2}};
        REQUIRE (actual == reference);
    }

    SECTION ("permuts works with three different sets")
    {
        auto actual = Permutation::generatePermutations ({2, 3, 4});
        std::set<std::vector<size_t>> reference {{0, 0, 0}, {0, 0, 1}, {0, 0, 2}, {0, 0, 3}, {0, 1, 0}, {0, 1, 1},
                                                 {0, 1, 2}, {0, 1, 3}, {0, 2, 0}, {0, 2, 1}, {0, 2, 2}, {0, 2, 3},
                                                 {1, 0, 0}, {1, 0, 1}, {1, 0, 2}, {1, 0, 3}, {1, 1, 0}, {1, 1, 1},
                                                 {1, 1, 2}, {1, 1, 3}, {1, 2, 0}, {1, 2, 1}, {1, 2, 2}, {1, 2, 3}};
        REQUIRE (actual == reference);
    }
}