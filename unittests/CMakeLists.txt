MESSAGE(STATUS "Unittests")

FILE(GLOB HDRS "*.h")
FILE(GLOB SRC "*.cpp")

add_executable(bpstore_unittests ${HDRS} ${SRC})
TARGET_LINK_LIBRARIES(bpstore_unittests ${bpstore_LIBRARIES} ${CMAKE_DL_LIBS})
TARGET_LINK_DIRECTORIES(bpstore_unittests PUBLIC ${bpstore_LIBRARY_DIR})
target_include_directories(bpstore_unittests PUBLIC ${bpstore_INCLUDE_DIR})
target_compile_definitions(bpstore_unittests PUBLIC  -DCATCH_CONFIG_ENABLE_BENCHMARKING)

add_test(NAME bpstore_unittests COMMAND bpstore_unittests) 
set_target_properties(bpstore_unittests PROPERTIES FOLDER Tests)
PRECOMPILED_HEADERS(bpstore_unittests)
