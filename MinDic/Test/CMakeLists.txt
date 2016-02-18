
IF( CPPUNIT_FOUND )
 add_executable( testMinDic testMinDic.cxx )
 target_link_libraries( testMinDic  ${CPPUNIT_LIBRARIES} )

 ADD_TEST( testMinDic ${EXECUTABLE_OUTPUT_PATH}/testMinDic )

ENDIF( CPPUNIT_FOUND )
