add_library( CorrectionSystem STATIC CorrectionSystem.cxx ${ICU_LIBRARIES})

add_executable( execCorrectionSystem execCorrectionSystem.cxx )
target_link_libraries( execCorrectionSystem OCRCorrection ${ICU_LIBRARIES})

add_executable( testCorrectionSystem testCorrectionSystem.cxx )
target_link_libraries( testCorrectionSystem OCRCorrection ${ICU_LIBRARIES})


add_executable(  prepareDocument exec_prepareDocument.cxx )
target_link_libraries( prepareDocument OCRCorrection ${ICU_LIBRARIES})
