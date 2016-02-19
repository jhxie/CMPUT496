# aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROTOTYPE_SRCS)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
#set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
add_executable(timestamp timestamp.cpp)
#target_link_libraries(timestamp ${CMAKE_THREAD_LIBS_INIT})
install(TARGETS timestamp
		RUNTIME DESTINATION bin      COMPONENT Runtime)
