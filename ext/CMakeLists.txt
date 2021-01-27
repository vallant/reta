add_library(CLI11 INTERFACE)
target_include_directories(CLI11 INTERFACE CLI11/include)

add_library(json INTERFACE)
target_include_directories(json INTERFACE json/single_include)

add_subdirectory(Catch2)

add_subdirectory(dither-cxx)
target_include_directories(dither PUBLIC dither-cxx/include)
if(MSVC)
  target_compile_options(dither PUBLIC /wd4100 /wd4267)
endif()

add_library(cpp-httplib INTERFACE)
target_include_directories(cpp-httplib INTERFACE cpp-httplib)

add_subdirectory(juce)

if(AAX_PATH)
  juce_set_aax_sdk_path(${AAX_PATH})
endif()

if(VST_PATH)
  juce_set_vst2_sdk_path(${VST_PATH})
endif()

# cmake-format: off
juce_add_plugin(frequalizer
                VERSION 0.1.2
                COMPANY_NAME vallant
                PLUGIN_MANUFACTURER_CODE Vall
                PLUGIN_CODE Prpl
                FORMATS VST3
                PRODUCT_NAME frequalizer)  
# cmake-format: on

target_include_directories(frequalizer PRIVATE Frequalizer/JuceLibraryCode ./JuceLibraryCode)
target_sources(
  frequalizer
  PRIVATE Frequalizer/Source/Analyser.h
          Frequalizer/Source/FrequalizerEditor.cpp
          Frequalizer/Source/FrequalizerEditor.h
          Frequalizer/Source/FrequalizerProcessor.cpp
          Frequalizer/Source/FrequalizerProcessor.h
          Frequalizer/Source/SocialButtons.h
          Frequalizer/JuceLibraryCode/BinaryData.h
          Frequalizer/JuceLibraryCode/BinaryData.cpp)
target_link_libraries(frequalizer PRIVATE juce::juce_opengl juce::juce_dsp juce::juce_audio_utils)
target_compile_definitions(frequalizer PUBLIC JUCE_VST3_CAN_REPLACE_VST2=0 JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)

reta_ifelse(MSVC COMPILER_FLAGS "/wd4244" "")
target_compile_options(frequalizer PRIVATE ${COMPILER_FLAGS})
target_compile_options(frequalizer PUBLIC -fprofile-instr-generate -fcoverage-mapping)
target_link_libraries(frequalizer PUBLIC -fprofile-instr-generate -fcoverage-mapping)

get_target_property(ARTEFACTS_DIR frequalizer_VST3 LIBRARY_OUTPUT_DIRECTORY)
add_custom_command(
  TARGET frequalizer_VST3
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${ARTEFACTS_DIR} ${CMAKE_BINARY_DIR}/bin/)
set_target_properties(frequalizer frequalizer_VST3 PROPERTIES EXCLUDE_FROM_ALL TRUE)
