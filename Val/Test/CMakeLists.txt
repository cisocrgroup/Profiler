IF( CPPUNIT_FOUND )
 add_executable( testVal testVal.cxx )
 target_link_libraries( testVal Pattern Utils ${CPPUNIT_LIBRARIES} )

ENDIF( CPPUNIT_FOUND )
