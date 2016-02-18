add_executable( abbyy_transform exec_abbyy_transform.cxx )
target_link_libraries( abbyy_transform Pattern OCRCorrection Pattern)

add_executable( testAbbyyXmlParser testAbbyyXmlParser.cxx )
target_link_libraries( testAbbyyXmlParser Pattern OCRCorrection Pattern)
