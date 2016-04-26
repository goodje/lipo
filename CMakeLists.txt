cmake_minimum_required(VERSION 2.8)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../../bin)

include_directories(
)

link_directories(
)

link_libraries(
)

set(SOURCE_LIB
        #server
        lib_socket.c
        lib_socket.h
        )

set(SOURCE_FILES_SERVER
        #server
        server_lipo.h
        server_lipo.c
        )
add_executable(lipod ${SOURCE_FILES_SERVER} ${SOURCE_LIB})

set(SOURCE_FILES_CLIENT
        #service
        client_lipo.h
        client_lipo.c
)
add_executable(lipo ${SOURCE_FILES_CLIENT} ${SOURCE_LIB})


#add_library(liblipo SHARED ${SOURCE_FILES_SERVER})
