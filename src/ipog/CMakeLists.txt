target_sources(reta_common PRIVATE DitherAdapter.h DitherAdapter.cpp)

if(USE_ACTS)
  target_sources(reta_common PRIVATE Acts.cpp Acts.h ActsConfigAdapter.cpp ActsConfigAdapter.h)
endif()
