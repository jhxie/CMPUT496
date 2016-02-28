# aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROTOTYPE_SRCS)
add_executable(ts ts.cpp biowrapper.cpp timestamp.cpp cmnutil.cpp)
#target_link_libraries(timestamp ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(ts ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES})
install(TARGETS ts
		RUNTIME DESTINATION bin      COMPONENT Runtime)
	#LIBRARY DESTINATION lib      COMPONENT Runtime
	#ARCHIVE DESTINATION lib/timestamp COMPONENT Development)
