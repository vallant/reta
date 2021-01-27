#include "Permutation.h"

void Permutation::recurse (size_t combinationSize, const std::vector<std::vector<size_t>>& indices,
                           std::vector<size_t>& stack, std::set<std::vector<size_t>>& combinationSet)
{
    if ( stack.size() == combinationSize )
    {
        combinationSet.insert (stack);
        return;
    }

    // Add a new number, iterate over all possibilities
    auto indicesPerIndex = indices[stack.size()];
    stack.push_back (0);
    for ( size_t number : indicesPerIndex )
    {
        stack.back() = number;
        recurse (combinationSize, indices, stack, combinationSet);
    }
    stack.pop_back();
}

std::set<std::vector<size_t>> Permutation::generatePermutations (size_t elements, size_t permutationSize)
{
    std::vector<size_t> sizes;
    for ( size_t i = 0; i < permutationSize; ++i )
        sizes.push_back (elements);

    return generatePermutations (sizes);
}

std::set<std::vector<size_t>> Permutation::generatePermutations (const std::vector<size_t>& elementSizes)
{
    std::vector<size_t> stack;
    std::vector<std::vector<size_t>> indices;
    std::set<std::vector<size_t>> combinationSet;

    for ( auto e : elementSizes )
    {
        std::vector<size_t> indicesPerIndex;
        for ( size_t i = 0; i < e; ++i )
            indicesPerIndex.push_back (i);
        indices.push_back (indicesPerIndex);
    }

    recurse (elementSizes.size(), indices, stack, combinationSet);
    return combinationSet;
}
