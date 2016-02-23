include_directories (${csl_SOURCE_DIR})
link_directories (${csl_BINARY_DIR}/csl)

add_executable(compileMD compileMD.cxx)
target_link_libraries (compileMD OCRCorrection)

install(
  TARGETS compileMD
  DESTINATION bin

  )
