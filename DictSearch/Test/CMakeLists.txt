IF( CPPUNIT_FOUND )
add_executable( testDictSearch exec_testDictSearch.cxx )
target_link_libraries( testDictSearch Utils DictSearch LevDEA Pattern INIConfig ${CPPUNIT_LIBRARIES})
ADD_TEST( testDictSearch ${EXECUTABLE_OUTPUT_PATH}/testDictSearch )

ENDIF( CPPUNIT_FOUND )
