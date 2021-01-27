#pragma once

#include <set>
#include <vector>

/* A class for computing permutations */
class Permutation
{
public:
    /* Compute all permutation for numElements digits, each of which can take numChoices different values */
    static std::set<std::vector<size_t>> generatePermutations (size_t numChoices, size_t numElements);

    /* Compute all permutations. Each element in the input vector specifies how many choices this element has. Inspect
     * the tests for more information. */
    static std::set<std::vector<size_t>> generatePermutations (const std::vector<size_t>& elementChoices);

private:
    static void recurse (size_t combinationSize, const std::vector<std::vector<size_t>>& indices,
                         std::vector<size_t>& stack, std::set<std::vector<size_t>>& combinationSet);
};