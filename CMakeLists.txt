project(OCRCorrection_cxx)

cmake_minimum_required(VERSION 2.4)





ADD_CUSTOM_TARGET(postcorrection DEPENDS JNICorrectionSystem )



#SET(CMAKE_VERBOSE_MAKEFILE true)

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
# the RPATH to be used when installing
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")



SET(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-unused -DXML_LIBRARY " )
SET(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -O3 " )
SET(CMKAE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -Wdisabled-optimization")
#SET(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -O1 " )
#SET(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -O0 -ggdb " )
#SET(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -g " )

SET( LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib )
SET( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )

###### JNI ###############

#INCLUDE(FindJNI) # FINDS THE FOLLOWING VARIABLES
#  JAVA_AWT_LIB_PATH     = the path to the jawt library
#  JAVA_JVM_LIB_PATH     = the path to the jvm library
#  JAVA_INCLUDE_PATH     = the include path to jni.h
#  JAVA_INCLUDE_PATH2    = the include path to jni_md.h
#  JAVA_AWT_INCLUDE_PATH = the include path to jawt.h

INCLUDE_DIRECTORIES( "." ${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2} )

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")

##### CPPUNIT ##############
SET( USER_SOFTWARE_DIR /mounts/Users/student/uli/soft )
#  Finds:
#   CPPUNIT_INCLUDE_DIR
#   CPPUNIT_LIBRARIES
FIND_PACKAGE( CPPUNIT )
IF( CPPUNIT_FOUND )
 INCLUDE_DIRECTORIES( ${CPPUNIT_INCLUDE_DIR} )
ENDIF( CPPUNIT_FOUND )

######## XERCES ###############
FIND_PACKAGE( XERCES REQUIRED)
IF( XERCES_FOUND )
 INCLUDE_DIRECTORIES( ${XERCES_INCLUDE_DIR} )
ENDIF( XERCES_FOUND )

add_subdirectory(AbbyyXmlParser)
add_subdirectory(Alphabet)
add_subdirectory(AltoXML)
add_subdirectory(CorrectionSystem)
add_subdirectory(DictSearch)
add_subdirectory(DocEvaluation)
add_subdirectory(DocXML)
add_subdirectory(Document)
add_subdirectory(FBDic)
add_subdirectory(FBDicString)
add_subdirectory(Getopt)
add_subdirectory(GlobalProfile)
add_subdirectory(INIConfig)
add_subdirectory(LevDEA)
add_subdirectory(LevenshteinWeights)
add_subdirectory(MinDic)
add_subdirectory(Pattern)
add_subdirectory(Profiler)
add_subdirectory(SimpleEnrich)
add_subdirectory(TXTReader)
add_subdirectory(Utils)
add_subdirectory(Vaam)
add_subdirectory(Val)
add_subdirectory(markup)
add_subdirectory(tools)

###### TO DO: Move this (and all related headers and sources) to a separate folder ##########
SET( OCRCORRECTION_SOURCES
             Utils/Utils.cxx
	     Document/Document.cxx
	     CorrectionSystem/CorrectionSystem.cxx
	     LevenshteinWeights/LevenshteinWeights.cxx
	     Profiler/Profiler.cxx Profiler/HTMLWriter.cxx Profiler/Evaluation.cxx Profiler/SimpleOutputWriter.cxx
	     AbbyyXmlParser/AbbyyXmlParser.cxx
	     DocXML/DocXMLReader.cxx
	     DocXML/LegacyDocXMLReader.cxx
	     DocXML/DocXMLWriter.cxx
	     DocXML/MergedGroundtruthReader.cxx
	     AltoXML/AltoXMLReader.cxx
	     AltoXML/AltoEnrich.cxx
	     TXTReader/TXTReader.cxx
	     TXTReader/AlignedTXTReader.cxx
	     Token/TokenImageInfoBox.cxx
	     GlobalProfile/GlobalProfileXMLReader.cxx
	     GlobalProfile/PatternContainerXMLReader.cxx

	     )

add_library( OCRCorrection STATIC ${OCRCORRECTION_SOURCES})
target_link_libraries(OCRCorrection LevDEA Pattern INIConfig DictSearch)

# BEGIN ICU Library
find_package(ICU 4.4 REQUIRED)
include_directories(
    ${ICU_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
#add_executable(tables tables.c)
#target_link_libraries(tables ${ICU_LIBRARIES})
# END ICU Library

target_link_libraries( OCRCorrection
  ${ICU_LIBRARIES}
  DictSearch
  )

install(
  TARGETS OCRCorrection
  DESTINATION lib
  )

IF( CPPUNIT_FOUND )
target_link_libraries( OCRCorrection
  ${CPPUNIT_LIBRARIES}
  )
ENDIF( CPPUNIT_FOUND )

IF( XERCES_FOUND )
target_link_libraries( OCRCorrection
  ${XERCES_LIBRARIES}
  )
ENDIF( XERCES_FOUND )



#############################


# The JNI shared lib
# add_library(JNICorrectionSystem SHARED
# 		${OCRCORRECTION_SOURCES}
# 	    jni/jav_JNITest.cxx
# 	    jni/jav_correctionBackend_CorrectionSystem.cxx
# 	    jni/jav_correctionBackend_candidateExtractor_CandidateExtractor.cxx
# 	    jni/jav_correctionBackend_document_Candidate.cxx
# 	    jni/jav_correctionBackend_document_Document.cxx
# 	    jni/jav_correctionBackend_document_Token.cxx
# 	    jni/jav_correctionBackend_document_Token_CandIterator.cxx
# 	    jni/jav_correctionBackend_document_Page.cxx
# 	    jni/jav_correctionBackend_CandidateSet.cxx
# 	    )
#target_link_libraries( JNICorrectionSystem ${CSL_LIBRARY} )
#target_link_libraries( JNICorrectionSystem ${XERCES_LIBRARIES} )
