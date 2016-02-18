include_directories (${csl_SOURCE_DIR})
link_directories (${csl_BINARY_DIR}/csl)

add_executable(extractMDString extractMDString.cxx)
target_link_libraries (extractMDString OCRCorrection)

install(
  TARGETS extractMDString
  DESTINATION bin
  )
