cmake_minimum_required(VERSION 2.8)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../../bin)

find_package(Threads REQUIRED)


include_directories(
)

link_directories(
)

link_libraries(
)

set(SOURCE_LIB
        lib_socket.c
        lib_socket.h
        )

set(SOURCE_FILES_MORTISE
        mortise.h
        mortise.c
        )
add_executable(mortise ${SOURCE_FILES_MORTISE} ${SOURCE_LIB})

set(SOURCE_FILES_TENON
        tenon.h
        tenon.c
)
add_executable(tenon ${SOURCE_FILES_TENON} ${SOURCE_LIB})
target_link_libraries(tenon ${CMAKE_THREAD_LIBS_INIT})

#add_library(liblipo SHARED ${SOURCE_FILES_SERVER})
