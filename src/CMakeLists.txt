add_library(libsdb libsdb.cpp)
add_library(sdb::libsdb ALIAS libsdb)

set_target_properties(
    libsdb
    PROPERTIES OUTPUT_NAME sdb
)

target_compile_features(libsdb PUBLIC cxx_std_17)

target_include_directories(libsdb
    PUBLIC
        $<INSTALL_INTERFACE:include> 
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include> 
    PRIVATE 
        ${CMAKE_SOURCE_DIR}/src/include
)

# Install the library and add it to the export set used above.
install(TARGETS libsdb
  EXPORT sdb-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}   # .so / .dylib
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}   # .a
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}   # .dll on Windows
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)