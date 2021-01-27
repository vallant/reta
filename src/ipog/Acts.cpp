#include "Acts.h"

#include "ActsConfigAdapter.h"

#include <BinaryData.h>
#include <jni.h>
#include <util/Util.h>

void Acts::call (const juce::String& jarPath, const juce::String& input, const juce::String& output, int interactions)
{
    auto jarPathOption = juce::String (classPathOption) + jarPath;

    auto vmArgs = createArgs ({jarPathOption, outputFormatOption, degree + juce::String (interactions)});

    JavaVM* jvm;
    JNIEnv* env;

    JNI_CreateJavaVM (&jvm, reinterpret_cast<void**> (&env), &vmArgs);

    auto actsClass = env->FindClass (className);
    auto mainMethod = env->GetStaticMethodID (actsClass, mainMethodName, mainMethodSignature);

    auto args = createStringArray (env, {"cmd", input, output});
    env->CallStaticVoidMethod (actsClass, mainMethod, args);

    for ( int i = 0; i < vmArgs.nOptions; ++i )
    {
        delete[] vmArgs.options[i].optionString;
    }
    delete[] vmArgs.options;

    jvm->DestroyJavaVM();
}

std::vector<TestConfiguration> Acts::generate (FileSystem* fs, const Plugin::Info& info,
                                               const GenerateCommand::Parameters& parameters)
{
    auto tempDir = juce::File::getSpecialLocation (juce::File::tempDirectory).getFullPathName();
    auto acts = tempDir + juce::File::getSeparatorString() + "acts.jar";
    if ( ! fs->exists (acts) )
    {
        auto stream = fs->createOutputStream (acts, true);

        stream->write (BinaryData::acts_jar, BinaryData::acts_jarSize);
    }

    auto inputFile = tempDir + juce::File::getSeparatorString() + juce::Uuid().toString();
    auto outputFile = tempDir + juce::File::getSeparatorString() + juce::Uuid().toString();

    std::vector<Plugin::ParameterInfo> params;
    for ( auto& param : info.parameters )
    {
        if ( ! util::matchedByAnyWildcard (parameters.exclude, param.name) )
            params.emplace_back (param);
    }

    auto actsInput = ActsConfigAdapter::inputToActs (info.name, params, info.layouts);

    fs->write (inputFile, actsInput, true);
    call (acts, inputFile, outputFile, parameters.interactions);

    auto actsConfig = fs->readString (outputFile);
    return ActsConfigAdapter::configFromActs (params, info.layouts, actsConfig);
}

jobjectArray Acts::createStringArray (JNIEnv* env, const std::vector<juce::String>& content)
{
    auto javaStringClass = env->FindClass (stringClassName);
    auto array = env->NewObjectArray (static_cast<int> (content.size()), javaStringClass, NULL);
    for ( size_t i = 0; i < content.size(); ++i )
    {
        auto option = content[i].toStdString();
        auto value = env->NewStringUTF (option.c_str());
        env->SetObjectArrayElement (array, static_cast<jsize> (i), value);
    }
    return array;
}

JavaVMInitArgs Acts::createArgs (const std::vector<juce::String>& options)
{
    JavaVMInitArgs args;
    JavaVMOption* vmOptions = new JavaVMOption[options.size()];
    memset (vmOptions, 0, options.size() * sizeof (JavaVMOption));

    for ( size_t i = 0; i < options.size(); ++i )
    {
        auto option = options[i].toStdString();
        auto length = strlen (option.c_str()) + 1;
        vmOptions[i].optionString = new char[length];
        std::fill (vmOptions[i].optionString, vmOptions[i].optionString + length, '\0');
        option.copy (vmOptions[i].optionString, length - 1);
    }
#ifdef JNI_VERSION_1_8
    args.version = JNI_VERSION_1_8;
#else
    args.version = JNI_VERSION_1_6;
#endif
    args.nOptions = static_cast<int> (options.size());
    args.options = vmOptions;
    args.ignoreUnrecognized = true;
    return args;
}
