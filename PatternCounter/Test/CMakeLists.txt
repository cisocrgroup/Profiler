IF( CPPUNIT_FOUND )
 add_executable( testPatternCounter exec_testPatternCounter.cxx )
 target_link_libraries( testPatternCounter  OCRCorrection ${CPPUNIT_LIBRARIES} )

ENDIF( CPPUNIT_FOUND )
