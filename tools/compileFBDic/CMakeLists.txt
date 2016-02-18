include_directories (${csl_SOURCE_DIR})
link_directories (${csl_BINARY_DIR}/csl)

add_executable(compileFBDic compileFBDic.cxx)
target_link_libraries (compileFBDic OCRCorrection)

install(
  TARGETS compileFBDic
  DESTINATION bin
)
