add_executable( altoEnrich exec_altoEnrich.cxx)
target_link_libraries ( altoEnrich OCRCorrection )


add_subdirectory( Test )

install( 
  TARGETS altoEnrich
  DESTINATION bin
  )
