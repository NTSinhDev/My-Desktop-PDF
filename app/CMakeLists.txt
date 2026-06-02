find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(MyDesktopPDF WIN32
  main.cpp
  application_context.cpp
)

target_include_directories(MyDesktopPDF PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(MyDesktopPDF
  PRIVATE
    Qt6::Widgets
    mdpdf_ui
    mdpdf_modules
    mdpdf_infrastructure
    mdpdf_core
)

include(MdpdfHelpers)
mdpdf_configure_target(MyDesktopPDF)

if(WIN32)
  set_target_properties(MyDesktopPDF PROPERTIES
    OUTPUT_NAME "MyDesktopPDF"
  )
endif()
