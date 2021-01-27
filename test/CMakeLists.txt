# https://forum.juce.com/t/httpclient-app-transport-security-policy-issues-on-os-x/16023/10
set(PLIST "<plist><dict><key>NSAppTransportSecurity</key><dict><key>NSAllowsLocalNetworking</key><true/></dict></dict></plist>")

juce_add_gui_app(reta_test PRODUCT_NAME reta_test PLIST_TO_MERGE ${PLIST})

add_subdirectory(plugin)
add_subdirectory(util)
add_subdirectory(test)
add_subdirectory(buffer)
add_subdirectory(ipog)
add_subdirectory(fs)
add_subdirectory(correlation)
add_subdirectory(assets)
add_subdirectory(server)

target_sources(reta_test PRIVATE Basic.cpp main.cpp)
target_link_libraries(reta_test PRIVATE reta_common juce::juce_audio_plugin_client Catch2::Catch2)

add_dependencies(reta_test reta_plugin_VST3)
set_target_properties(reta_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
reta_group_target_sources(reta_test)

