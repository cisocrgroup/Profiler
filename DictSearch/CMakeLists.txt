add_library( DictSearch STATIC DictSearch.cxx AdaptiveLex.cxx)
target_link_libraries(DictSearch LevDEA INIConfig)
add_subdirectory( Test )
