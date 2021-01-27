function(reta_add_helper_targets)
  reta_ifelse(BRANCH_NAME CPPCHECK_REPORTER "--xml" "")
  reta_ifelse(WIN32 CATCH_XML "${CMAKE_BINARY_DIR}/catch_win.xml" "${CMAKE_BINARY_DIR}/catch_mac.xml")

  find_package(
    Python3
    COMPONENTS Interpreter
    REQUIRED)

  add_custom_target(
    run_catch
    COMMAND "$<TARGET_FILE:reta_test>" "--durations" "yes" "-r" "junit" "-o" "${CATCH_XML}"
    DEPENDS reta_test)

  add_custom_target(
    run_python_test
    COMMAND ${Python3_EXECUTABLE} "-m" "unittest" "discover" "-p" "*test*.py"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

  # add_custom_target(run_tests DEPENDS run_catch run_python_test)
  add_custom_target(run_tests DEPENDS run_catch)

  find_program(
    CPPCHECK
    NAMES cppcheck.exe cppcheck
    PATHS "C:\\Program Files\\Cppcheck" "/usr/local/bin/")
    
  if(CPPCHECK)
    add_custom_target(
      run_analysis
      ${CPPCHECK}
      "--enable=warning,style,performance,portability"
      "--language=c++"
      "--force"
      "--quiet"
      "--inline-suppr"
      "--suppressions-list=.cppcheck-suppressions"
      ${CPPCHECK_REPORTER}
      "src"
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
  endif()

  add_custom_target(
    run_clang_format
    COMMAND ${Python3_EXECUTABLE} "ext/run-clang-format/run-clang-format.py" "-r" "-i" "src/" "test/"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

  find_program(CMAKE_FORMAT NAMES cmake-format)
  if(CMAKE_FORMAT)
    set(CMAKELISTS)
    foreach(DIR in LISTS src test resources evaluation cmake)
      file(GLOB_RECURSE CMAKELISTS_DIR ${CMAKE_SOURCE_DIR}/${DIR}/*/CMakeLists.txt)
      list(APPEND CMAKELISTS ${CMAKELISTS_DIR})
    endforeach()

    list(
      APPEND
      CMAKELISTS
      ${CMAKE_SOURCE_DIR}/CMakeLists.txt
      ${CMAKE_SOURCE_DIR}/ext/CMakeLists.txt
      ${CMAKE_SOURCE_DIR}/cmake/HelperTargets.cmake
      ${CMAKE_SOURCE_DIR}/cmake/Utils.cmake
      ${CMAKE_SOURCE_DIR}/resources/CMakeLists.txt)

    add_custom_target(
      run_cmake_format
      COMMAND ${CMAKE_FORMAT} "-i" ${CMAKELISTS}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    add_custom_target(run_format DEPENDS run_cmake_format run_clang_format)
  else()
    add_custom_target(run_format DEPENDS run_clang_format)
  endif()

endfunction()
