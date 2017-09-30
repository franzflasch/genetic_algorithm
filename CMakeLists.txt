project (genetic)
cmake_minimum_required(VERSION 2.8)

### Common Settings
set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")

### SHARED Library
set(LIB_SRCS src/genetic.c)
add_library(genetic SHARED ${LIB_SRCS})
set_target_properties(genetic PROPERTIES PUBLIC_HEADER inc/genetic.h)
set_target_properties(genetic PROPERTIES SOVERSION "1")
target_link_libraries(genetic m)
install(TARGETS genetic DESTINATION lib PUBLIC_HEADER DESTINATION inc)

### Example executable
include_directories(inc)
add_executable(genetic_example src/main.c)
target_link_libraries(genetic_example genetic)

### genann genetic evolution example
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ~/MEGA/ArtificialIntelligence/snake_ai/genann/build)
include_directories(~/MEGA/ArtificialIntelligence/snake_ai/genann)
add_executable(genetic_evo src/genann_evo.c)
target_link_libraries(genetic_evo genetic)
find_library(GENANN genann)
target_link_libraries(genetic_evo "${GENANN}")

### Debian Package generation
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_VERSION "0.1.0")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Franz Flasch")
include(CPack)
