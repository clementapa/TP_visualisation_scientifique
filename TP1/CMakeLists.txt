cmake_minimum_required(VERSION 2.8)
project(TP1)
set(CMAKE_BUILD_TYPE "Release")

set(VTK_DIR "/home/clement/Cours/5A/Visualisation_scientifique/VTK-build/")

#list(APPEND CMAKE_PREFIX_PATH "/home/clement/Cours/5A/Visualisation_scientifique/VTK-build/")

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

include_directories(".")

add_executable(TP1
    main.cpp)
    
set_target_properties(TP1
    PROPERTIES
    COMPILE_FLAGS
    "-march=native -O3")
    
target_link_libraries(TP1
    ${VTK_LIBRARIES}) 

install(TARGETS TP1 RUNTIME DESTINATION bin)
