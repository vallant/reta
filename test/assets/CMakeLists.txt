# cmake-format: off
juce_add_plugin(reta_plugin
                VERSION 0.1.2
                COMPANY_NAME vallant
                PLUGIN_MANUFACTURER_CODE Vall
                PLUGIN_CODE Prpl
                FORMATS VST3
                PRODUCT_NAME reta)
# cmake-format: on

target_sources(reta_plugin PRIVATE PluginEditor.cpp PluginProcessor.cpp)
target_link_libraries(reta_plugin PRIVATE juce::juce_audio_processors)
target_compile_definitions(reta_plugin PUBLIC JUCE_VST3_CAN_REPLACE_VST2=0 JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)

if(WIN32)
  add_custom_command(
    TARGET reta_plugin_VST3
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:reta_plugin_VST3>
            ${CMAKE_BINARY_DIR}/bin/$<$<CONFIG:Debug>:Debug/>$<$<CONFIG:Release>:Release/>reta.vst3)
elseif(APPLE)
  add_custom_command(
    TARGET reta_plugin_VST3
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory $<TARGET_BUNDLE_DIR:reta_plugin_VST3>/
            ${CMAKE_BINARY_DIR}/bin/$<$<CONFIG:Debug>:Debug/>$<$<CONFIG:Release>:Release/>reta.vst3)
endif()
