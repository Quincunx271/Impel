cmake_minimum_required(VERSION 3.16)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(Impel
  VERSION 0.0.1
  LANGUAGES CXX
  DESCRIPTION "Good Description"
)

include(is_root)
is_root(is_root_project)

set(IMPEL_DEVELOPER_DEFAULTS "${is_root_project}" CACHE BOOL "Default all options to developer-friendly values")

option(BUILD_TESTING "Enable testing" ${IMPEL_DEVELOPER_DEFAULTS})
option(IMPEL_TEST_COLOR "Force test color" OFF)

if(BUILD_TESTING)
  enable_testing()
endif()

if(IMPEL_DEVELOPER_DEFAULTS)
  include(pmm.cmake)
  pmm(CONAN)
endif()

# Header only library. Changes needed for non-header-only
add_library(Impel INTERFACE)
add_library(Impel::Impel ALIAS Impel)
target_include_directories(Impel
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(Impel
  INTERFACE
    cxx_std_20
)

# Tests
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

# Install
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR "${CMAKE_INSTALL_LIBDIR}/cmake/Impel")

install(TARGETS Impel
  EXPORT Impel-Targets
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

install(EXPORT Impel-Targets
  FILE ImpelTargets.cmake
  NAMESPACE Impel::
  DESTINATION "${INSTALL_CONFIGDIR}"
)

install(DIRECTORY include/Impel/
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/Impel"
  FILES_MATCHING PATTERN "*.hpp"
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/ImpelConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY SameMinorVersion
  ARCH_INDEPENDENT
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/ImpelConfigVersion.cmake"
  DESTINATION "${INSTALL_CONFIGDIR}"
)
