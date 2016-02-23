include_directories (${csl_SOURCE_DIR})
link_directories (${csl_BINARY_DIR}/csl)

add_executable(lookupMD lookupMD.cxx)
target_link_libraries (lookupMD OCRCorrection)

install(
  TARGETS lookupMD
  DESTINATION bin
  )
