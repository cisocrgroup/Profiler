
IF( CPPUNIT_FOUND )
 add_executable( testFBDicString testFBDicString.cxx )
 target_link_libraries( testFBDicString  ${CPPUNIT_LIBRARIES} )

 ADD_TEST( testFBDicString ${EXECUTABLE_OUTPUT_PATH}/testFBDicString )

ENDIF( CPPUNIT_FOUND )
