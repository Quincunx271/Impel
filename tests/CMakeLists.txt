find_package(Catch2 REQUIRED)

# Set up warnings / similar flags
set(werr ${IMPEL_WARNINGS_AS_ERRORS})

if(CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
  add_compile_options(
    /permissive- /W4
    $<$<BOOL:${werr}>:/WX>
  )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL GNU)
  add_compile_options(
    -Wall -Wextra -Wpedantic
    $<$<BOOL:${werr}>:-Werror>
  )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL Clang)
  add_compile_options(
    -Wall -Wextra -Wpedantic
    $<$<BOOL:${werr}>:-Werror>
  )
endif()

file(GLOB_RECURSE test_sources CONFIGURE_DEPENDS "*.test.cpp")

add_executable(test.Impel ${test_sources})
target_link_libraries(test.Impel
  PRIVATE
    Impel::Impel
    Catch2::Catch2
)
target_compile_features(test.Impel
  PRIVATE
    cxx_std_20
)

include(Catch)

catch_discover_tests(test.Impel
  EXTRA_ARGS $<$<BOOL:${IMPEL_TEST_COLOR}>:--use-colour=yes>
)
