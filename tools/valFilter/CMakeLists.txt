add_executable(valFilter valFilter.cxx)
target_link_libraries (valFilter OCRCorrection)

add_executable(valEvaluation valEvaluation.cxx)
target_link_libraries (valEvaluation OCRCorrection)

install(
  TARGETS valFilter
  DESTINATION bin

  )
