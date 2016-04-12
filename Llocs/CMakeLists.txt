add_executable(profile-xwords profileXWords.cxx)
target_link_libraries (profile-xwords OCRCorrection)

install(
  TARGETS profile-xwords
  DESTINATION bin
  )
