MESSAGE(STATUS "Benchmarks")

macro(ADD_BENCHARK name src)
        MESSAGE(STATUS " +" ${name})
        add_executable(benchmark_${name} ${src})
        TARGET_LINK_LIBRARIES(benchmark_${name} ${rstore_LIBRARIES} ${CMAKE_DL_LIBS})
	    TARGET_LINK_DIRECTORIES(benchmark_${name} PUBLIC ${rstore_LIBRARY_DIR})
	    target_include_directories(benchmark_${name} PUBLIC ${rstore_INCLUDE_DIR})
		set_target_properties(benchmark_${name} PROPERTIES FOLDER benchmarks)

		if(MSVC)
			target_compile_options(benchmark_${name} PUBLIC  /wd"4702")
		endif(MSVC)
endmacro(ADD_BENCHARK)

ADD_BENCHARK(rw rw_bench.cpp)

#add_executable(rw_bench rw_bench.cpp)
#TARGET_LINK_LIBRARIES(rw_bench ${rstore_LIBRARIES} ${CMAKE_DL_LIBS})
#TARGET_LINK_DIRECTORIES(rw_bench PUBLIC ${rstore_LIBRARY_DIR})
#target_include_directories(rw_bench PUBLIC ${rstore_INCLUDE_DIR})

#set_target_properties(rw_bench PROPERTIES FOLDER benchmarks)

#if(MSVC)
  #target_compile_options(rw_bench PUBLIC  /wd"4702")
#endif(MSVC)
