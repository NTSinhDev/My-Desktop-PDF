find_package(spdlog CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(pdfium CONFIG REQUIRED)
if(TARGET pdfium::pdfium)
  set(MDPDF_PDFIUM_LIB pdfium::pdfium)
elseif(TARGET pdfium)
  set(MDPDF_PDFIUM_LIB pdfium)
else()
  message(FATAL_ERROR "pdfium target not found")
endif()

add_library(mdpdf_infrastructure STATIC
  pdfium/pdfium_library.cpp
  pdfium/pdfium_engine.cpp
  pdfium/pdfium_renderer.cpp
  pdfium/pdfium_text_search.cpp
  pdfium/pdfium_text_selection.cpp
  cache/lru_page_cache.cpp
  persistence/sqlite_database.cpp
  persistence/bookmark_repository.cpp
  persistence/recent_files_repository.cpp
  threading/render_thread_pool.cpp
  logging/logger.cpp
)

target_include_directories(mdpdf_infrastructure PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(mdpdf_infrastructure
  PUBLIC
    mdpdf_core
    spdlog::spdlog
    unofficial::sqlite3::sqlite3
    ${MDPDF_PDFIUM_LIB}
)

include(MdpdfHelpers)
mdpdf_configure_target(mdpdf_infrastructure)
