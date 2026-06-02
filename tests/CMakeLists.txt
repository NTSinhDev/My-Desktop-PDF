find_package(GTest CONFIG REQUIRED)

add_executable(mdpdf_tests
  core/lru_page_cache_test.cpp
  core/document_models_test.cpp
)

target_link_libraries(mdpdf_tests
  PRIVATE
    GTest::gtest_main
    mdpdf_core
    mdpdf_infrastructure
)

include(GoogleTest)
gtest_discover_tests(mdpdf_tests)
