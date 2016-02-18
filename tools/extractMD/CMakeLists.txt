include_directories (${csl_SOURCE_DIR})
link_directories (${csl_BINARY_DIR}/csl)

add_executable(extractMD extractMD.cxx)
target_link_libraries (extractMD OCRCorrection)

install(
  TARGETS extractMD
  DESTINATION bin

  )
