SET ( OCRCORRECTION_IMPL ${OCRCORRECTION_IMPL} TEST)
ADD_LIBRARY(INIConfig STATIC dictionary.c INIConfig.cxx INIParser.cxx)
target_link_libraries(INIConfig Utils)
ADD_SUBDIRECTORY( Test )
