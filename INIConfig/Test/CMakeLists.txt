
IF( CPPUNIT_FOUND )
 add_executable( testINIConfig exec_testINIConfig.cxx )
 target_link_libraries(testINIConfig Utils INIConfig ${CPPUNIT_LIBRARIES})
 ADD_TEST(testINIConfig ${EXECUTABLE_OUTPUT_PATH}/testINIConfig)
ENDIF( CPPUNIT_FOUND )
