#pragma once
#include <algorithm>
#include <juce_core/juce_core.h>
#include <vector>
namespace util
{
/* Generate a vector of indices from a vector: (a,b,c) -> (0,1,2) */
template <typename T>
static std::vector<int> vecToIndices (const std::vector<T>& vec)
{
    std::vector<int> indices;
    indices.resize (vec.size());
    for ( size_t i = 0; i < vec.size(); ++i )
        indices[i] = static_cast<int> (i);
    return indices;
}

/* Check if a vector contains the provided element */
template <typename T>
static bool contains (const std::vector<T>& vec, const T& val)
{
    return std::find (vec.begin(), vec.end(), val) != vec.end();
}

/* Check if a string matches any of a vector of wildcards */
static inline bool matchedByAnyWildcard (const std::vector<std::string>& wildcards, const std::string& value)
{
    auto pred = [&value] (const std::string& wildcard) {
        return juce::String (value).matchesWildcard (wildcard, true);
    };
    return std::find_if (wildcards.begin(), wildcards.end(), pred) != wildcards.end();
}

/* Join a set of strings into a single string */
static inline std::string join (const std::vector<std::string>& input, const std::string& joinString)
{
    juce::StringArray array;
    array.addArray (input.begin(), input.end());
    return array.joinIntoString (joinString).toStdString();
}
}  // namespace util