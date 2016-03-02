# aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROTOTYPE_SRCS)
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
SET(BUILD_SHARED_LIBRARIES OFF)
SET(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
add_executable(ts ts.cpp timestamp.cpp cmnutil.cpp)
#target_link_libraries(ts ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES})
install(TARGETS ts
		RUNTIME DESTINATION /usr/bin      COMPONENT Runtime)
	#LIBRARY DESTINATION lib      COMPONENT Runtime
	#ARCHIVE DESTINATION lib/timestamp COMPONENT Development)
