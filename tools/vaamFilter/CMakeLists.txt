include_directories (${csl_SOURCE_DIR})
link_directories (${csl_BINARY_DIR}/csl)

add_executable(vaamFilter vaamFilter.cxx)
target_link_libraries (vaamFilter OCRCorrection)

add_executable(vaamEvaluation vaamEvaluation.cxx)
target_link_libraries (vaamEvaluation OCRCorrection)

install(
  TARGETS vaamFilter
  DESTINATION bin

  )

add_executable(vaamFilter_TT vaamFilter.cxx)
target_link_libraries (vaamFilter_TT OCRCorrection)
set_target_properties( vaamFilter_TT PROPERTIES COMPILE_FLAGS -DCSL_VAAMFILTER_USE_TRANSTABLE )
install(
  TARGETS vaamFilter_TT
  DESTINATION bin

  )
