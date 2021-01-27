target_sources(reta_test PRIVATE DitherAdapterTest.cpp)
if(USE_ACTS)
  target_sources(reta_test PRIVATE ActsConfigAdapterTest.cpp)
endif()
