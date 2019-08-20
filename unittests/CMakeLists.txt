MESSAGE(STATUS "Unittests")

FILE(GLOB HDRS "*.h")
FILE(GLOB SRC "*.cpp")

add_executable(rstore_unittests ${HDRS} ${SRC})
TARGET_LINK_LIBRARIES(rstore_unittests ${rstore_LIBRARIES} ${CMAKE_DL_LIBS})
TARGET_LINK_DIRECTORIES(rstore_unittests PUBLIC ${rstore_LIBRARY_DIR})
target_include_directories(rstore_unittests PUBLIC ${rstore_INCLUDE_DIR})
target_compile_definitions(rstore_unittests PUBLIC  -DCATCH_CONFIG_ENABLE_BENCHMARKING)

add_test(NAME rstore_unittests COMMAND rstore_unittests) 
set_target_properties(rstore_unittests PROPERTIES FOLDER Tests)
PRECOMPILED_HEADERS(rstore_unittests)
