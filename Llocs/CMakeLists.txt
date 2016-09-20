add_executable(prof-xwords profileXWords.cxx)
target_link_libraries (prof-xwords OCRCorrection)
add_executable(prof-fishy profFishy.cxx)
target_link_libraries (prof-fishy OCRCorrection)
add_executable(prof-bestlines profBestLines.cxx)
target_link_libraries (prof-bestlines OCRCorrection)

install(
  TARGETS prof-xwords prof-bestlines prof-fishy
  DESTINATION bin
  )
