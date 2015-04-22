add_executable( abbyy_transform exec_abbyy_transform.cxx )
target_link_libraries( abbyy_transform OCRCorrection )

add_executable( testAbbyyXmlParser testAbbyyXmlParser.cxx )
target_link_libraries( testAbbyyXmlParser OCRCorrection )

