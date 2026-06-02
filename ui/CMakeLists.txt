find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

add_library(mdpdf_ui STATIC
  views/main_window.cpp
  views/pdf_scroll_view.cpp
  utils/bitmap_utils.cpp
  resources/resources.qrc
)

target_include_directories(mdpdf_ui PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(mdpdf_ui
  PUBLIC
    Qt6::Widgets
    mdpdf_core
    mdpdf_modules
    mdpdf_infrastructure
)

include(MdpdfHelpers)
mdpdf_configure_target(mdpdf_ui)
