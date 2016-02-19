# aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROTOTYPE_SRCS)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
#set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
add_executable(tssend tssend.cpp)
#add_executable(tsrecv tsrecv.cpp)
#target_link_libraries(tssend ${CMAKE_THREAD_LIBS_INIT})
#target_link_libraries(tsrecv ${CMAKE_THREAD_LIBS_INIT})
install(TARGETS tssend #tsrecv
		RUNTIME DESTINATION bin      COMPONENT Runtime)
