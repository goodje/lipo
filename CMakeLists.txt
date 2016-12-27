cmake_minimum_required(VERSION 2.8)

message("-- CMAKE_SYSTEM_INFO_FILE: ${CMAKE_SYSTEM_INFO_FILE}")
message("-- CMAKE_SYSTEM_NAME:      ${CMAKE_SYSTEM_NAME}")
message("-- CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
message("-- CMAKE_SYSTEM:           ${CMAKE_SYSTEM}")

string (REGEX MATCH "\\.el[1-9]" os_version_suffix ${CMAKE_SYSTEM})
message("-- os_version_suffix:      ${os_version_suffix}")

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
        src/network/http.c
        src/network/http.h
        src/event/lipo_event.h
        src/event/event_epoll.c
        src/event/event_kqueue.c
        src/logging/logging.c
        src/logging/logging.h
        src/utils.c
        src/utils.h
        #--
        src/mortise/mortise_tenon.h
        src/mortise/mortise_tenon.c
        #--
        )

#TODO should > 2.6 maybe
if(CMAKE_SYSTEM MATCHES "^Linux")
    set(SOURCE_FILES_MORTISE
            src/mortise.h
            src/mortise.c
            )
    add_executable(mortise ${SOURCE_FILES_MORTISE} ${SOURCE_LIB})
endif()


set(SOURCE_FILES_TENON
        src/tenon.h
        src/tenon.c
        src/tenon/tenon_mortise.c
        src/tenon/tenon_mortise.h
        src/event/event_manage.c
        src/event/event_manage.h)
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

#add_executable(test
#        test/test.c)
