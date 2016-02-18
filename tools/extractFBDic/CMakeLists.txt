include_directories (${csl_SOURCE_DIR})
link_directories (${csl_BINARY_DIR}/csl)

add_executable(extractFBDic extractFBDic.cxx)
target_link_libraries (extractFBDic OCRCorrection)


install(
  TARGETS extractFBDic
  DESTINATION bin

  )
