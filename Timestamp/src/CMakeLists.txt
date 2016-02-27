# aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROTOTYPE_SRCS)
add_executable(timestamp timestamp.cpp)
#target_link_libraries(timestamp ${CMAKE_THREAD_LIBS_INIT})
install(TARGETS timestamp
		RUNTIME DESTINATION bin      COMPONENT Runtime)
	#LIBRARY DESTINATION lib      COMPONENT Runtime
	#ARCHIVE DESTINATION lib/timestamp COMPONENT Development)
