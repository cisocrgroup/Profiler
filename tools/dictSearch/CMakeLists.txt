include_directories (${csl_SOURCE_DIR})
link_directories (${csl_BINARY_DIR}/csl)

add_executable(runDictSearch runDictSearch.cxx)
target_link_libraries (runDictSearch OCRCorrection)

install(
  TARGETS runDictSearch
  DESTINATION bin
  )
