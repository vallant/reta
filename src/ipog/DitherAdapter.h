#pragma once

#include "test/TestConfiguration.h"

#include <command/GenerateCommand.h>

/* A class to generate test configurations using the dither-cxx library */
class DitherAdapter
{
public:
    /* Generate the test configurations using the provided plugin */
    static TestConfigurations generate (Plugin* plugin, const GenerateCommand::Parameters& parameters);
    static TestConfigurations generate (const Plugin::Info& info, const GenerateCommand::Parameters& parameters);
};