find_package(Catch2 REQUIRED)

# Set up warnings / similar flags
set(MSVC_flags /permissive-)
set(Clang_flags -Wall -Wextra -Wpedantic)
set(GNU_flags -Wall -Wextra -Wpedantic)

file(GLOB_RECURSE test_sources CONFIGURE_DEPENDS "*.test.cpp")

add_executable(test.Impel ${test_sources})
target_link_libraries(test.Impel
  PRIVATE
    Impel::Impel
    Catch2::Catch2
)
target_compile_options(test.Impel
  PRIVATE
    ${${CMAKE_CXX_COMPILER_ID}_flags}
)
target_compile_features(test.Impel
  PRIVATE
    cxx_std_20
)

include(Catch)

catch_discover_tests(test.Impel
  EXTRA_ARGS $<$<BOOL:${IMPEL_TEST_COLOR}>:--use-colour=yes>
)
