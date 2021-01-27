#define CATCH_CONFIG_RUNNER
#include "App.h"
#include "catch2/catch.hpp"
#include "juce_events/juce_events.h"

class RetaTest : public App
{
public:
    RetaTest() : App ("Reta_test") {}
    int execute (int argc, char const* const argv[]) override
    {
        return Catch::Session().run (argc, argv);
    }
};

START_JUCE_APPLICATION (RetaTest)