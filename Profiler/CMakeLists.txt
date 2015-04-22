
add_executable(profiler exec_profiler.cxx)
target_link_libraries (profiler OCRCorrection )

add_executable(trainFrequencyList exec_trainFrequencyList.cxx)
target_link_libraries (trainFrequencyList ${CSL_LIBRARIES} OCRCorrection )

add_executable(queryFrequencyList exec_queryFrequencyList.cxx)
target_link_libraries (queryFrequencyList ${CSL_LIBRARIES} OCRCorrection )

#add_executable(ocr_analysis_groundtruth ocr_analysis_groundtruth.cxx)
#target_link_libraries (ocr_analysis_groundtruth ${CSL_LIBRARIES} OCRCorrection )

add_executable(createHistoricLexicon exec_createHistoricLexicon.cxx)
target_link_libraries (createHistoricLexicon OCRCorrection )

add_executable(guessTraces exec_guessTraces.cxx)
target_link_libraries (guessTraces OCRCorrection )

install( 
  TARGETS profiler
  DESTINATION bin
  )

add_subdirectory ( Test ) 
