cmake_minimum_required(VERSION 3.19)

project("sdb" LANGUAGES CXX)

include(CTest)

add_subdirectory("src")
add_subdirectory("tools")

if(BUILD_TESTING)
    find_package(Catch2 CONFIG REQUIRED)
    add_subdirectory("test")
endif()

# Install public headers (the whole include/ tree).
install(
  DIRECTORY ${PROJECT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export the targets defined in src/ as a CMake package
# This writes sdb-targets.cmake into the install tree
install(
  EXPORT sdb-targets
  FILE sdb-config.cmake
  NAMESPACE sdb::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sdb
)