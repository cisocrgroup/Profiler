include_directories (${csl_SOURCE_DIR})
link_directories (${csl_BINARY_DIR}/csl)

add_executable(compileMDString compileMDString.cxx)
target_link_libraries (compileMDString OCRCorrection)

install(
  TARGETS compileMDString
  DESTINATION bin

  )
