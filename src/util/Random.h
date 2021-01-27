#pragma once
#include <ctime>
#include <juce_core/juce_core.h>

/* A class to simplify access to random numbers */
class Random
{
public:
    /* Initialize the engine with the provided seed */
    Random (size_t seed = static_cast<size_t> (std::time (nullptr))) : generator (static_cast<unsigned int> (seed)) {}

    /* Return a value between the interval (min, max), including the boundaries */
    template <typename T>
    T next (T min, T max)
    {
        if constexpr ( std::is_floating_point_v<T> )
        {
            return min + generator.nextFloat() * (max - min);
        }
        else
        {
            if ( min == 0 && max == 0 )
                return 0;
            return generator.nextInt (juce::Range (static_cast<int> (min), static_cast<int> (max + 1)));
        }
    }

    /* Choose a random element of the vector */
    template <typename T>
    T nextFrom (const std::vector<T>& vec)
    {
        return vec[next (static_cast<size_t> (0), vec.size() - 1)];
    }

private:
    juce::Random generator;
};