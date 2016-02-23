add_executable( altoEnrich exec_altoEnrich.cxx)
target_link_libraries ( altoEnrich OCRCorrection Pattern INIConfig Pattern LevDEA DictSearch LevDEA )


add_subdirectory( Test )

install(
  TARGETS altoEnrich
  DESTINATION bin
  )
