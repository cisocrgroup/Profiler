include_directories (${csl_SOURCE_DIR})
link_directories (${csl_BINARY_DIR}/csl)

add_executable(lookupMD2 lookupMD2.cxx)
target_link_libraries (lookupMD2 OCRCorrection)

install(
  TARGETS lookupMD2
  DESTINATION bin
  )
