add_executable( ocrc_transform exec_ocrc_transform.cxx)
target_link_libraries ( ocrc_transform OCRCorrection )

add_executable(mergeGroundtruth exec_mergeGroundtruth.cxx)
target_link_libraries (mergeGroundtruth OCRCorrection )

add_executable(readLegacy_docXML exec_readLegacy_docXML.cxx)
target_link_libraries (readLegacy_docXML OCRCorrection )

add_subdirectory( Test )

install(
  TARGETS mergeGroundtruth ocrc_transform
  DESTINATION bin
  )
