IF( CPPUNIT_FOUND )
 add_executable( testLevDistance exec_testLevDistance.cxx )
 target_link_libraries( testLevDistance  csl ${CPPUNIT_LIBRARIES} )

 ADD_TEST( testLevDistance ${EXECUTABLE_OUTPUT_PATH}/testLevDistance )

ENDIF( CPPUNIT_FOUND )
