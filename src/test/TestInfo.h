#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

/* Struct holding information of a single test */
struct TestInfo
{
    std::string name;
    std::string expected;
    std::string actual;
    bool passed;
    int configIndex = -1;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE (TestInfo, name, expected, actual, passed, configIndex)
};

using TestInfos = std::vector<TestInfo>;

/* Struct holding information of a complete comparison run */
struct TestResult
{
    TestInfos basicTests;
    TestInfos busLayoutTests;
    std::vector<TestInfos> parameterTests;
    std::vector<TestInfos> processingTests;
    std::vector<TestInfos> configTests;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE (TestResult, basicTests, busLayoutTests, parameterTests, configTests,
                                    processingTests)
};

/* Create a test comparing a member to another */
#define RETA_CREATE_TEST(name)                                                                                   \
    {                                                                                                            \
        std::string (#name) + " matches", std::to_string (name), std::to_string (other.name), name == other.name \
    }

/* Create a test comparing a float member to another */
#define RETA_CREATE_TEST_FLOAT(name)                                                          \
    {                                                                                         \
        std::string (#name) + " matches", std::to_string (name), std::to_string (other.name), \
            juce::approximatelyEqual (name, other.name)                                       \
    }

/* Create a test comparing a string member to another */
#define RETA_CREATE_TEST_STR(name)                                             \
    {                                                                          \
        std::string (#name) + " matches", name, other.name, name == other.name \
    }
