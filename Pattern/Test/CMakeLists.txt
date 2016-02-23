IF( CPPUNIT_FOUND )
  add_executable( testPattern testPattern.cxx )
  target_link_libraries( testPattern Utils Pattern ${CPPUNIT_LIBRARIES} )

  add_executable( testComputeInstruction exec_testComputeInstruction.cxx )
  target_link_libraries( testComputeInstruction Pattern Utils ${CPPUNIT_LIBRARIES} )

  ADD_TEST( testPattern ${EXECUTABLE_OUTPUT_PATH}/testPattern )
  ADD_TEST( testComputeInstruction ${EXECUTABLE_OUTPUT_PATH}/testPattern )

ENDIF( CPPUNIT_FOUND )

add_executable( patterns2dot exec_patterns2dot.cxx )
target_link_libraries( patterns2dot Pattern Utils ${CPPUNIT_LIBRARIES} )
