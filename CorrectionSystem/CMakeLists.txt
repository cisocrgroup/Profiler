add_library( CorrectionSystem STATIC CorrectionSystem.cxx )

add_executable( execCorrectionSystem execCorrectionSystem.cxx )
target_link_libraries( execCorrectionSystem OCRCorrection )

add_executable( testCorrectionSystem testCorrectionSystem.cxx )
target_link_libraries( testCorrectionSystem OCRCorrection )


add_executable(  prepareDocument exec_prepareDocument.cxx )
target_link_libraries( prepareDocument OCRCorrection )
