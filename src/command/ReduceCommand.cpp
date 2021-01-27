#include "ReduceCommand.h"

#include <correlation/Correlation.h>
#include <fs/FileSystem.h>
#include <test/TestCase.h>
#include <util/Permutation.h>

ReduceCommand::ReduceCommand (CLI::App* app, int& returnValue, FileSystem* fs)
{
    auto reduce = app->add_subcommand ("reduce", "Try to identify the root cause of an regression error");
    reduce->add_option ("-r,--reference", parameters.reference, "Path to reference plugin")->required();
    reduce->add_option ("-a,--actual", parameters.actual, "Path to test plugin")->required();
    reduce->add_option ("-l,--log", parameters.results, "Path to output file from the 'compare' command")->required();
    reduce
        ->add_option ("-c,--config", parameters.config,
                      "Path to config that was used for testing reference and test plugin")
        ->required();
    reduce->add_option ("-o,--output", parameters.output, "Path to output JSON")->required();
    reduce->add_flag ("-v,--verbose", parameters.verbose, "Log results to console");
    reduce->parse_complete_callback (
        [&returnValue, fs, &parameters = parameters] { returnValue = execute (parameters, fs); });
}

int ReduceCommand::execute (const Parameters& parameters, FileSystem* fs)
{
    std::cout << "Reducing failures from results at path: " << parameters.results << std::endl;
    std::cout << "Config that was used when observing failure: " << parameters.config << std::endl;
    std::cout << "Reference: " << parameters.reference << ", actual: " << parameters.actual << std::endl;
    std::cout << "Saving results at path: " << parameters.output << std::endl;

    auto configs = fs->readJson<TestConfigurations> (parameters.config);
    auto results = fs->readJson<TestResult> (parameters.results);
    auto reference = fs->openPlugin (parameters.reference);
    auto actual = fs->openPlugin (parameters.actual);

    TestConfigurations reducedConfigs;
    for ( const auto& testSet : results.processingTests )
    {
        for ( const auto& individualTest : testSet )
        {
            if ( ! individualTest.passed )
            {
                auto reduced = ddmin (configs[individualTest.configIndex], reference.get(), actual.get());
                reducedConfigs.push_back (reduced);
            }
        }
    }

    if ( reducedConfigs.empty() )
        std::cout << "Warning: no failed tests found." << std::endl;

    fs->writeJson (parameters.output, reducedConfigs, true);

    if ( parameters.verbose )
        printReducedConfigs (reducedConfigs);

    return 0;
}

void ReduceCommand::printReducedConfigs (const TestConfigurations& reduced)
{
    std::cout << "Could reduce " << reduced.size() << " test configurations" << std::endl;
    for ( size_t i = 0; i < reduced.size(); ++i )
    {
        std::cout << "#" << i << std::endl;
        std::cout << "  Blocksize: " << reduced[i].blockSize << std::endl;
        std::cout << "  Signal: " << reduced[i].inputSignal << std::endl;
        std::cout << "  SampleRate: " << reduced[i].sampleRate << std::endl;
        std::cout << "  BusLayout: " << reduced[i].layout.getDescription() << std::endl;
        std::cout << "  Parameters: " << std::endl;
        for ( auto& parameter : reduced[i].parameters )
            std::cout << "     - " << parameter.name << ": " << parameter.value << std::endl;
    }
}

// https://blog.acolyer.org/2015/11/16/simplifying-and-isolating-failure-inducing-input/
// https://github.com/hydraseq/ddmin/blob/master/ddmin/ddmin.py
// https://www.researchgate.net/publication/291418636_Isolating_Graphical_Failure-Inducing_Input_for_Privacy_Protection_in_Error_Reporting_Systems

TestConfiguration ReduceCommand::ddmin (const TestConfiguration& failingPartition, Plugin* reference, Plugin* actual,
                                        size_t numPartitions)
{
    if ( failingPartition.parameters.size() < numPartitions )
        return failingPartition;

    auto partitions = split (failingPartition, numPartitions);
    for ( auto& partition : partitions )
    {
        if ( failed (partition, reference, actual) )
            return ddmin (partition, reference, actual, 2);
    }

    auto complements = computeComplements (failingPartition, partitions);
    for ( auto& complement : complements )
    {
        if ( failed (complement, reference, actual) )
            return ddmin (complement, reference, actual, 2);
    }

    return ddmin (failingPartition, reference, actual, numPartitions * 2);
}

bool ReduceCommand::failed (const TestConfiguration& config, Plugin* reference, Plugin* actual)
{
    auto referenceResult = TestCase::execute (reference, config);
    auto actualResult = TestCase::execute (actual, config);
    auto diff = Correlation::rmse (referenceResult.first, actualResult.first);
    auto result = referenceResult.second.compareTo (actualResult.second);
    auto anyFailed = std::find_if (result.begin(), result.end(), [] (const TestInfo& test) { return ! test.passed; });
    return diff > 0.001 || anyFailed != result.end();
}

TestConfigurations ReduceCommand::split (const TestConfiguration& partition, size_t numPartitions)
{
    if ( partition.parameters.size() < numPartitions )
        return {partition};

    TestConfigurations partitions (numPartitions, partition);
    size_t carry = partition.parameters.size() % numPartitions;
    size_t partitionSize = partition.parameters.size() / numPartitions;
    size_t partitionBeginIndex = 0;

    for ( size_t i = 0; i < numPartitions; ++i )
    {
        auto curPartSize = partitionSize;
        if ( carry-- > 0 )
            curPartSize++;
        auto begin = partition.parameters.begin() + partitionBeginIndex;
        auto end =
            partition.parameters.begin() + std::min (partitionBeginIndex + curPartSize, partition.parameters.size());
        partitions[i].parameters.clear();
        partitions[i].parameters.insert (partitions[i].parameters.begin(), begin, end);
        partitionBeginIndex += curPartSize;
    }
    return partitions;
}

TestConfigurations ReduceCommand::computeComplements (const TestConfiguration& initialConfig,
                                                      const TestConfigurations& partitions)
{
    TestConfigurations complements (partitions.size(), initialConfig);
    for ( size_t i = 0; i < partitions.size(); ++i )
    {
        auto& parameters = complements[i].parameters;
        parameters.clear();
        for ( auto& param : initialConfig.parameters )
        {
            auto it = std::find_if (
                partitions[i].parameters.begin(), partitions[i].parameters.end(), [param] (const auto& element) {
                    return element.name == param.name && juce::approximatelyEqual (element.value, param.value);
                });
            if ( it == partitions[i].parameters.end() )
                parameters.push_back (param);
        }
    }
    return complements;
}