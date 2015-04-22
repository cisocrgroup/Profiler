
IF( CPPUNIT_FOUND )
 add_executable( testDocument exec_testDocument.cxx )
 target_link_libraries( testDocument  ${CPPUNIT_LIBRARIES} )
 target_link_libraries( testDocument  OCRCorrection )

 ADD_TEST( testDocument ${EXECUTABLE_OUTPUT_PATH}/testDocument )



 add_executable( quickTest quickTest.cxx )
 target_link_libraries( quickTest  ${CPPUNIT_LIBRARIES} )
 target_link_libraries( quickTest  OCRCorrection )

ENDIF( CPPUNIT_FOUND )
