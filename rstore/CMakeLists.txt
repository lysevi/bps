set(PROJECT "rstore")
project (${PROJECT} VERSION 0.0.1 LANGUAGES CXX)

include(GenerateExportHeader)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/config.h.cmake ${PROJECT}/config.h)


FILE(GLOB GLOB_HDRS "*.h")
FILE(GLOB UTILS_HDRS "utils/*.h")

FILE(GLOB GLOB_SRC "*.cpp")
FILE(GLOB UTILS_SRC "utils/*.cpp")


source_group(\\ FILES ${GLOB_HDRS} ${GLOB_SRC})
source_group(utils FILES ${UTILS_HDRS} ${UTILS_SRC})

SET(SRC ${GLOB_HDRS} ${GLOB_SRC} 
    ${UTILS_HDRS}  ${UTILS_SRC}
)


add_library(rstore SHARED ${SRC})
TARGET_LINK_LIBRARIES(rstore  ${rstore_PRIVATE_LIBRARIES})
target_link_directories(rstore PUBLIC ${rstore_LIBRARY_DIR})
target_include_directories(rstore PUBLIC ${rstore_INCLUDE_DIR})
add_library(rstore::rstore ALIAS rstore)

PRECOMPILED_HEADERS(rstore)

#IF(WIN32)
# SET(rstore_NET_LIBRARIES  ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} wsock32 ws2_32 )
#else()
# SET(rstore_NET_LIBRARIES ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )
#endif()

SET(rstore_LIBRARIES rstore ${rstore_PRIVATE_LIBRARIES} ${rstore_NET_LIBRARIES} CACHE STRING "rstore libs")


GENERATE_EXPORT_HEADER(rstore
    BASE_NAME rstore
    EXPORT_MACRO_NAME EXPORT
    EXPORT_FILE_NAME exports.h
    STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)
