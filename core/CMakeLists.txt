add_library(mdpdf_core STATIC
  domain/document_models.cpp
  events/event_bus.cpp
)

target_include_directories(mdpdf_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

include(MdpdfHelpers)
mdpdf_configure_target(mdpdf_core)
