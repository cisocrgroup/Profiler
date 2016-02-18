include_directories (${csl_SOURCE_DIR})
link_directories (${csl_BINARY_DIR}/csl)

add_executable(lookupMDString lookupMDString.cxx)
target_link_libraries (lookupMDString OCRCorrection)

install(
  TARGETS lookupMDString
  DESTINATION bin

  )
