cmake_minimum_required (VERSION 3.10)

# Set options for this project.
#project(tests)
project (server ${PROJECT_NAME})

set(SOURCES
    main.cpp)
set(HEADERS
    include/servertcp.hpp
    include/array2d.hpp
    include/matrix_mult.hpp
    include/matrix_initialised.hpp)




# Set options used by "CMake".
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth=4250")

include_directories(include)

find_package(Boost ${BOOST_MIN_VERSION} COMPONENTS context coroutine date_time timer thread system program_options regex REQUIRED)

# "Add an executable to the project using the specified source files."
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# SET(CMAKE_BUILD_TOOL C:/MinGW/bin/mingw32-make.exe)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set_target_properties(${PROJECT_NAME}  PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
    #COMPILE_OPTIONS "-Wstrict"
)
# "Link a target to given libraries."
 target_link_libraries (${PROJECT_NAME} ${Boost_LIBRARIES})


