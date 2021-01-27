#pragma once
#include "jni.h"
#include "test/TestConfiguration.h"

#include <command/GenerateCommand.h>
#include <juce_core/juce_core.h>

/*  A class that encapsulates the logic necessary to make the JNI call to the ACTS jarfile */
class Acts
{
public:
    /* Perform the actual call to ACTS
        @param jarPath: Path where the acts.jar is located
        @param input: Path where the ACTS-Compatible input file is located
        @param output: Path where the ACTS output should be placed
        @param interactions: Combinatorial interactions that should be covered
    */
    static void call (const juce::String& jarPath, const juce::String& input, const juce::String& output,
                      int interactions);

    /*  Entry point of the call. Calls acts and returns the test configurations as a vector. */
    static std::vector<TestConfiguration> generate (FileSystem* fs, const Plugin::Info& info,
                                                    const GenerateCommand::Parameters& parameters);

private:
    /*  Create a JNI-Stringarray from a C++ string array */
    static jobjectArray createStringArray (JNIEnv* env, const std::vector<juce::String>& content);

    /*  Create the JNI-InitArgs from a C++ string array */
    static JavaVMInitArgs createArgs (const std::vector<juce::String>& options);

    /*  Java classname of the acts entrypoint */
    static constexpr auto className = "edu/uta/cse/fireeye/console/ActsConsoleManager";
    /*  Java classname of string */
    static constexpr auto stringClassName = "java/lang/String";

    /*  Java signature of the main method */
    static constexpr auto mainMethodSignature = "([Ljava/lang/String;)V";
    static constexpr auto mainMethodName = "main";
    static constexpr auto classPathOption = "-Djava.class.path=";

    /*  ACTS option to save the results in a csv format */
    static constexpr auto outputFormatOption = "-Doutput=csv";

    /*  ACTS option to specify the number of interactions that should be covered*/
    static constexpr auto degree = "-Ddoi=";
};
