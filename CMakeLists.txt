cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(hscpp)

set(CMAKE_CXX_STANDARD 11)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(LibHyperscan)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src/matcher)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src/pattern)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src/userctx)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src/util)

# set include path
if (HS_INCLUDE_PATH)
    set(${LibHyperscan_INCLUDE_DIR} HS_INCLUDE_PATH CACHE STRING "set include path" FORCE )
endif()

# set hs library to link
if (HS_Library_Name)
    set(${LibHyperscan_LIBRARIES} HS_Library_Name CACHE STRING "set hs library to link" FORCE )
endif()

# Set Build Type
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "Default build type 'Release with debug info'")
    set(CMAKE_BUILD_TYPE RELWITHDEBINFO CACHE STRING "" FORCE )
else()
    string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)
    message(STATUS "Build type ${CMAKE_BUILD_TYPE}")
endif()

if(CMAKE_BUILD_TYPE MATCHES RELEASE|RELWITHDEBINFO|MINSIZEREL)
    message(STATUS "using release build")
    set(RELEASE_BUILD TRUE)
else()
    set(RELEASE_BUILD FALSE)
endif()

set(BINDIR "${PROJECT_BINARY_DIR}/bin")
set(LIBDIR "${PROJECT_BINARY_DIR}/lib")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${LIBDIR}")

option(BUILD_SHARED_LIBS "Build shared libs instead of static" OFF)
option(BUILD_STATIC_AND_SHARED "Build shared libs as well as static" OFF)

if (BUILD_STATIC_AND_SHARED OR BUILD_SHARED_LIBS)
        message(STATUS "Building shared libraries")
else()
        message(STATUS "Building static libraries")
endif()

if (NOT BUILD_SHARED_LIBS)
    # build static libs
    set(BUILD_STATIC_LIBS ON)
    mark_as_advanced(BUILD_STATIC_LIBS)
endif ()


aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/matcher SRC_LIST) 
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/pattern SRC_LIST) 
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/userctx SRC_LIST) 
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/util SRC_LIST) 

message(${SRC_LIST})
if (BUILD_STATIC_LIBS)
    add_library(hscpp STATIC ${SRC_LIST} )
endif()

if (BUILD_STATIC_AND_SHARED OR BUILD_SHARED_LIBS)
    add_library(hscpp SHARED ${SRC_LIST} )
endif()

target_link_libraries(hscpp ${LibHyperscan_LIBRARIES})

SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 
set(CMAKE_INSTALL_RPATH ${LibHyperscan_LIBRARIES})

set(installable_libs hscpp)

install(TARGETS ${installable_libs} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib64)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/matcher/hs_matcher.h
                ${CMAKE_CURRENT_SOURCE_DIR}/src/matcher/matcher.h
                ${CMAKE_CURRENT_SOURCE_DIR}/src/pattern/hs_pattern.h
                ${CMAKE_CURRENT_SOURCE_DIR}/src/pattern/pattern.h
                ${CMAKE_CURRENT_SOURCE_DIR}/src/userctx/ctx.h
                ${CMAKE_CURRENT_SOURCE_DIR}/src/util/debug_log.h 
                ${CMAKE_CURRENT_SOURCE_DIR}/src/util/unique_id.h 
                DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

add_subdirectory(example)