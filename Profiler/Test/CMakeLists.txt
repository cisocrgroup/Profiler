IF( CPPUNIT_FOUND )
 add_executable( testFrequencyList exec_testFrequencyList.cxx )
 target_link_libraries( testFrequencyList  OCRCorrection ${CPPUNIT_LIBRARIES} )

ENDIF( CPPUNIT_FOUND )
