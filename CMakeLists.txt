cmake_minimum_required (VERSION 3.1)
project (decimal754)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ./lib)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ./lib)

add_compile_options(-ferror-limit=2)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(WINDOWS TRUE)
endif()

include_directories(./include ./IntelRDFPMathLib20U2/src/LIBRARY/src)
link_directories(./lib)


# === Intel Decimal Floating-Point Math Library === #
if (${MACOSX})
  set(INTEL_BUILD_COMMAND ./RUNOSXINTEL64)
  set(INTEL_INSTALL_COMMAND ln -s ${CMAKE_CURRENT_BINARY_DIR}/IntelRDFPMathLib20U2/src/LIBRARY/gcc000libbid.a ${CMAKE_CURRENT_BINARY_DIR}/lib/libbid.a)
elseif (LINUX)
  set(INTEL_BUILD_COMMAND ./RUNLINUX)
  set(INTEL_INSTALL_COMMAND ln -s ${CMAKE_CURRENT_BINARY_DIR}/IntelRDFPMathLib20U2/src/LIBRARY/gcc000libbid.a ${CMAKE_CURRENT_BINARY_DIR}/lib/libbid.a)
elseif (WINDOWS)
  set(INTEL_BUILD_COMMAND ./RUNWINDOWSINTEL64)
  set(INTEL_INSTALL_COMMAND ln -s ${CMAKE_CURRENT_BINARY_DIR}/IntelRDFPMathLib20U2/src/LIBRARY/gcc000libbid.dll ${CMAKE_CURRENT_BINARY_DIR}/lib/libbid.dll)
endif()

configure_file(CMakeLists.txt.in IntelRDFPMathLib20U2/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/IntelRDFPMathLib20U2)
if(result)
  message(FATAL_ERROR "CMake step for IntelRDFPMathLib20U2 failed: ${result}")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/IntelRDFPMathLib20U2)
if(result)
  message(FATAL_ERROR "Build step for IntelRDFPMathLib20U2 failed: ${result}")
endif()

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/IntelRDFPMathLib20U2 EXCLUDE_FROM_ALL)


# === DECIMAL754 === #
#add_library (decimal754 ./src/decimal.cpp)
#target_include_directories (decimal754 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(test ./src/main.cpp ./src/tests.cpp)
target_link_libraries (test LINK_PUBLIC bid)
