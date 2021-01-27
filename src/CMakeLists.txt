add_library(reta_common)

add_subdirectory(ipog)
add_subdirectory(util)
add_subdirectory(plugin)
add_subdirectory(fs)
add_subdirectory(buffer)
add_subdirectory(test)
add_subdirectory(correlation)
add_subdirectory(command)
add_subdirectory(server)


set(USE_ACTS OFF CACHE BOOL "Specify if acts should be made possible for generating")

target_sources(
  reta_common
  PRIVATE Reta.cpp Reta.h
  PUBLIC App.h App.cpp)
target_include_directories(reta_common PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(
  reta_common
  PRIVATE reta_binary juce::juce_audio_processors juce::juce_audio_utils
  PUBLIC CLI11 json dither cpp-httplib)

target_compile_definitions(
  reta_common
  PRIVATE JUCE_STANDALONE_APPLICATION=1
  PUBLIC JUCE_PLUGINHOST_VST3 JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)

if(USE_ACTS)
  target_compile_definitions(reta_common PRIVATE USE_ACTS=1)
  find_package(JNI 1.8 EXACT REQUIRED)
  target_include_directories(reta_common PRIVATE ${JNI_INCLUDE_DIRS})
  target_link_libraries(reta_common PRIVATE ${JNI_LIBRARIES})
endif()

reta_ifelse(MSVC COMPILER_FLAGS "/FC;/W4;/wd4244" "-Wall")
target_compile_options(reta_common PRIVATE ${COMPILER_FLAGS})
reta_group_target_sources(reta_common)

# https://forum.juce.com/t/httpclient-app-transport-security-policy-issues-on-os-x/16023/10
set(PLIST "<plist><dict><key>NSAppTransportSecurity</key><dict><key>NSAllowsLocalNetworking</key><true/></dict></dict></plist>")

juce_add_gui_app(reta PRODUCT_NAME reta PLIST_TO_MERGE ${PLIST})
target_link_libraries(reta PRIVATE reta_common juce::juce_core)
target_sources(reta PRIVATE main.cpp)

set_target_properties(reta PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
reta_group_target_sources(reta)