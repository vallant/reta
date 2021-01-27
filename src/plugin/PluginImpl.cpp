#include "PluginImpl.h"

PluginImpl::PluginImpl (juce::String fileName, std::unique_ptr<FileSystem> fs) :
    fs (std::move (fs)), fileName (std::move (fileName))
{
    if ( ! this->fs->exists (this->fileName) )
        throw std::runtime_error ("Path doesn't exist: " + fileName.toStdString());

    createPluginInstance();

    instance->getStateInformation (defaultConfig);
}

void PluginImpl::prepare (double sampleRate, size_t maxNumSamples, const juce::AudioProcessor::BusesLayout& layout)
{
    jassert (isLayoutSupported (layout));
    if ( layout != instance->getBusesLayout() )
    {
        instance->releaseResources();
        instance->setBusesLayout (layout);
    }

    if ( ! init || ! juce::approximatelyEqual (instance->getSampleRate(), sampleRate)
         || instance->getBlockSize() != static_cast<int> (maxNumSamples) )
    {
        instance->prepareToPlay (sampleRate, static_cast<int> (maxNumSamples));
        init = true;
    }
}

size_t PluginImpl::getNumParameters() const
{
    return static_cast<size_t> (instance->getParameters().size());
}

void PluginImpl::process (juce::AudioSampleBuffer& buffer)
{
    juce::MidiBuffer midi;
    instance->processBlock (buffer, midi);
}

bool PluginImpl::isLayoutSupported (const juce::AudioProcessor::BusesLayout& layout) const
{
    return instance->checkBusesLayoutSupported (layout);
}

std::pair<size_t, size_t> PluginImpl::getNumBuses() const
{
    return {instance->getBusCount (true), instance->getBusCount (false)};
}

Plugin::Info PluginImpl::getInfo() const
{
    const auto& p = static_cast<juce::AudioProcessor*> (instance.get());
    Info info {p->getName().toStdString(),
               p->getBusCount (true),
               p->getBusCount (false),
               p->acceptsMidi(),
               p->isMidiEffect(),
               p->producesMidi(),
               p->getLatencySamples(),
               p->getTailLengthSeconds(),
               p->getProcessingPrecision() == juce::AudioProcessor::ProcessingPrecision::singlePrecision ? "single"
                                                                                                         : "double",
               p->hasEditor()};

    info.layouts = getLayouts();
    info.parameters = getParameterInfo();

    return info;
}

std::vector<BusLayout> PluginImpl::getLayouts() const
{
    return BusLayout::getLayouts (this);
}

void PluginImpl::reset()
{
    instance->setStateInformation (defaultConfig.getData(), static_cast<int> (defaultConfig.getSize()));
}

void PluginImpl::createPluginInstance()
{
    juce::AudioPluginFormatManager formatManager;
    formatManager.addDefaultFormats();

    if ( fileName.endsWith (juce::File::getSeparatorString()) )
        fileName = fileName.upToLastOccurrenceOf (juce::File::getSeparatorString(), false, true);

    for ( auto& format : formatManager.getFormats() )
        format->findAllTypesForFile (plugins, fileName);

    if ( plugins.isEmpty() )
        throw std::runtime_error ("No compatible plugins were found at path: " + fileName.toStdString());

    juce::String error;
    instance = formatManager.createPluginInstance (*plugins[0], 44100.0, 1024, error);

    if ( instance == nullptr || error.isNotEmpty() )
        throw std::runtime_error ("Could not open plugin: " + fileName.toStdString()
                                  + ", Message: " + error.toStdString());
}

void PluginImpl::setParameter (const juce::String& name, float value)
{
    for ( auto& param : instance->getParameters() )
    {
        if ( param->getName (1000) == name && ! juce::approximatelyEqual (param->getValue(), value) )
        {
            param->setValueNotifyingHost (value);
            break;
        }
    }
}
std::vector<juce::String> PluginImpl::getParameterNames() const
{
    std::vector<juce::String> names;
    for ( auto& param : instance->getParameters() )
        names.emplace_back (param->getName (1000));
    return names;
}

juce::String PluginImpl::getCurrentParameterText (const juce::String& name) const
{
    for ( auto& param : instance->getParameters() )
    {
        if ( param->getName (1000) == name )
            return param->getCurrentValueAsText();
    }
    return "Unknown";
}

float PluginImpl::getValueFromText (const juce::String& name) const
{
    for ( auto& param : instance->getParameters() )
    {
        if ( param->getName (1000) == name )
            return param->getValueForText (param->getCurrentValueAsText());
    }
    return -1;
}

std::vector<Plugin::ParameterInfo> PluginImpl::getParameterInfo() const
{
    std::vector<Plugin::ParameterInfo> info;
    for ( auto& parameter : instance->getParameters() )
    {
        Plugin::ParameterInfo p {parameter->getName (1000).toStdString(),
                                 parameter->getLabel().toStdString(),
                                 parameter->getValue(),
                                 parameter->getNumSteps(),
                                 parameter->getCategory(),
                                 parameter->getDefaultValue(),
                                 parameter->getParameterIndex(),
                                 parameter->isAutomatable(),
                                 parameter->isDiscrete(),
                                 parameter->isMetaParameter(),
                                 parameter->isOrientationInverted(),
                                 parameter->isBoolean(),
                                 getDiscreteValues (parameter)};
        info.push_back (p);
    }

    return info;
}
std::vector<float> PluginImpl::getDiscreteValues (const juce::AudioProcessorParameter* parameter) const
{
    std::vector<float> values;
    if ( parameter->isDiscrete() )
    {
        auto maxIndex = static_cast<float> (parameter->getNumSteps() - 1);

        for ( int i = 0; i < parameter->getNumSteps(); ++i )
            values.push_back (i / maxIndex);
    }

    return values;
}
