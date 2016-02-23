
add_executable(msFilter msFilter.cxx)
target_link_libraries (msFilter OCRCorrection)

install(
  TARGETS msFilter
  DESTINATION bin
)
