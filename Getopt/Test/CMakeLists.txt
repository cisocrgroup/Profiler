
IF( CPPUNIT_FOUND )
 add_executable( testGetopt testGetopt.cxx )
 target_link_libraries( testGetopt  ${CPPUNIT_LIBRARIES} )

 ADD_TEST( testGetopt ${EXECUTABLE_OUTPUT_PATH}/testGetopt )

ENDIF( CPPUNIT_FOUND )
