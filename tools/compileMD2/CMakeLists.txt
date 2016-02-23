include_directories (${csl_SOURCE_DIR})
link_directories (${csl_BINARY_DIR}/csl)

add_executable(compileMD2 compileMD2.cxx)
target_link_libraries (compileMD2 OCRCorrection)

install(
  TARGETS compileMD2
  DESTINATION bin

  )
