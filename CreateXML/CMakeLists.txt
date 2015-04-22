add_library( CorrectionSystem STATIC CorrectionSystem.cxx )

add_executable( execCorrectionSystem execCorrectionSystem.cxx )
target_link_libraries( execCorrectionSystem OCRCorrection )
