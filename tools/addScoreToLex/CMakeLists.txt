include_directories (${csl_SOURCE_DIR})
link_directories (${csl_BINARY_DIR}/csl)

add_executable(addScoreToLex addScoreToLex.cxx)
target_link_libraries (addScoreToLex OCRCorrection)

install(
  TARGETS addScoreToLex
  DESTINATION bin
  )
