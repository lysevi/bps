set(PROJECT "bpstore")
project (${PROJECT} VERSION 0.0.1 LANGUAGES CXX)

include(GenerateExportHeader)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/config.h.cmake ${CMAKE_CURRENT_SOURCE_DIR}/config.h)


FILE(GLOB GLOB_HDRS "*.h")
FILE(GLOB UTILS_HDRS "utils/*.h")

FILE(GLOB GLOB_SRC "*.cpp")
FILE(GLOB UTILS_SRC "utils/*.cpp")


source_group(\\ FILES ${GLOB_HDRS} ${GLOB_SRC})
source_group(utils FILES ${UTILS_HDRS} ${UTILS_SRC})

SET(SRC ${GLOB_HDRS} ${GLOB_SRC} 
    ${UTILS_HDRS}  ${UTILS_SRC}
)


add_library(bpstore SHARED ${SRC})
TARGET_LINK_LIBRARIES(bpstore  ${bpstore_PRIVATE_LIBRARIES})
target_link_directories(bpstore PUBLIC ${bpstore_LIBRARY_DIR})
target_include_directories(bpstore PUBLIC ${bpstore_INCLUDE_DIR})
add_library(bpstore::bpstore ALIAS bpstore)

PRECOMPILED_HEADERS(bpstore)

#IF(WIN32)
# SET(bpstore_NET_LIBRARIES  ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} wsock32 ws2_32 )
#else()
# SET(bpstore_NET_LIBRARIES ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )
#endif()

SET(bpstore_LIBRARIES bpstore ${bpstore_PRIVATE_LIBRARIES} ${bpstore_NET_LIBRARIES} CACHE STRING "bpstore libs")


GENERATE_EXPORT_HEADER(bpstore
    BASE_NAME bpstore
    EXPORT_MACRO_NAME EXPORT
    EXPORT_FILE_NAME exports.h
    STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)
