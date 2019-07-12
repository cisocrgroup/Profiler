add_library( DictSearch STATIC DictSearch.cxx AdaptiveLex.cxx AdditionalLex.cxx UnknownVirtualLex.cxx GlobalHistPatterns.cpp)
target_link_libraries(DictSearch LevDEA INIConfig)
add_subdirectory( Test )
