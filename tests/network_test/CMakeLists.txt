cmake_minimum_required(VERSION 3.17)

project(NetworkTest LANGUAGES CXX)

set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

add_compile_definitions( BOOST_ALL_DYN_LINK )

get_filename_component( PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../.. ABSOLUTE )

FILE( GLOB_RECURSE HeadersFiles ${PARENT_DIR}/common/*.hpp ${PARENT_DIR}/network/*.hpp ${PARENT_DIR}/serialize/*.hpp ${PARENT_DIR}/tests/*.hpp )
FILE( GLOB_RECURSE SourceFiles ${PARENT_DIR}/common/*.cpp ${PARENT_DIR}/network/*.cpp ${PARENT_DIR}/serialize/*.cpp ${PARENT_DIR}/tests/*.cpp )

include_directories( ${PARENT_DIR} )

add_executable( NetworkTest ${HeadersFiles} ${SourceFiles} )

set( Boost_DEBUG ON )

# BOOST
if( WIN32 )
    set( BOOST_ROOT C:/local/boost_1_65_1 )
    set( BOOST_LIBRARYDIR ${BOOST_ROOT}/lib64-msvc-14.1 )
endif()

set( Boost_USE_MULTITHREADED ON )

find_package( Boost REQUIRED system filesystem thread log unit_test_framework )

if( Boost_FOUND )
    message( STATUS "found boost, Boost_LIBRARIES <" ${Boost_LIBRARIES} ">" )
    include_directories( ${Boost_INCLUDE_DIRS} )
    link_directories( ${Boost_LIBRARY_DIRS} )        
else()
    message( STATUS "boost not found" )
endif()

target_link_libraries( NetworkTest ${Boost_LIBRARIES} )
