
IF( CPPUNIT_FOUND )
 add_executable( testGlobalProfile exec_testGlobalProfile.cxx )
 target_link_libraries( testGlobalProfile  ${CPPUNIT_LIBRARIES} )
 target_link_libraries( testGlobalProfile  OCRCorrection )

 ADD_TEST( testGlobalProfile ${EXECUTABLE_OUTPUT_PATH}/testGlobalProfile )

ENDIF( CPPUNIT_FOUND )
