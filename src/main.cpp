#include "buffer/Buffer.h"
#include "util/Random.h"

#include <Reta.h>

struct P : App
{
    P() : App ("Reta") {}
    int execute (int argc, const char* const* argv) override
    {
        return Reta().execute (argc, argv);
    }
};

START_JUCE_APPLICATION (P);