add_library(mdpdf_modules STATIC
  document/document_service.cpp
  render/render_coordinator.cpp
)

target_include_directories(mdpdf_modules PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(mdpdf_modules
  PUBLIC
    mdpdf_core
    mdpdf_infrastructure
)

include(MdpdfHelpers)
mdpdf_configure_target(mdpdf_modules)
