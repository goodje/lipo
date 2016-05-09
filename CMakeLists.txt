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
        src/network/lib_socket.c
        src/network/lib_socket.h
        src/network/http.c src/network/http.h src/event/event.h src/event/event_epoll.c
        )

set(SOURCE_FILES_MORTISE
        src/mortise.h
        src/mortise.c
        )
add_executable(mortise ${SOURCE_FILES_MORTISE} ${SOURCE_LIB})

set(SOURCE_FILES_TENON
        src/tenon.h
        src/tenon.c
)
add_executable(tenon ${SOURCE_FILES_TENON} ${SOURCE_LIB})
target_link_libraries(tenon ${CMAKE_THREAD_LIBS_INIT})


#----- test
set(SOURCE_FILES_TEST_EVENT_CLIENT
        test/event_client.c
        test/event_client.h
)
add_executable(test_event_client ${SOURCE_FILES_TEST_EVENT_CLIENT} ${SOURCE_LIB})
target_link_libraries(test_event_client ${CMAKE_THREAD_LIBS_INIT})

#add_library(liblipo SHARED ${SOURCE_FILES_SERVER})
