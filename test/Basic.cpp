#include "App.h"

#include <catch2/catch.hpp>

TEST_CASE ("Test splitting of arguments", "[unit][TestApp]")
{
    auto actual = App::splitArguments ("a b c  d e", ' ');
    std::vector<std::string> reference = {"a", "b", "c", "d", "e"};
    REQUIRE (actual == reference);
}

TEST_CASE ("Test creation of argv", "[unit][TestApp]")
{
    std::vector<std::string> args = {"a", "b", "c", "d", "e"};
    auto argv = App::getArgv (args);
    REQUIRE (strcmp (argv[0], "a") == 0);
    REQUIRE (strcmp (argv[1], "b") == 0);
    REQUIRE (strcmp (argv[2], "c") == 0);
    REQUIRE (strcmp (argv[3], "d") == 0);
    REQUIRE (strcmp (argv[4], "e") == 0);
}

TEST_CASE ("Comparison of pointers works as expected", "[unit][TestApp]")
{
    std::unique_ptr<float> a (new float);
    std::unique_ptr<float> b (new float);

    *a = 0.0f;
    *b = 0.0f;

    REQUIRE_FALSE (a == b);
    REQUIRE (*a == *b);
}