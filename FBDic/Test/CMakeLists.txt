
IF( CPPUNIT_FOUND )
 add_executable( testFBDic testFBDic.cxx )
 target_link_libraries( testFBDic  ${CPPUNIT_LIBRARIES} )

 ADD_TEST( testFBDic ${EXECUTABLE_OUTPUT_PATH}/testFBDic )

ENDIF( CPPUNIT_FOUND )
