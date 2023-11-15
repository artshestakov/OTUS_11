cmake_minimum_required(VERSION 3.12)

project(join_server)
set(CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(Boost REQUIRED)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})

    add_executable(join_server
        src/main.cpp
        src/server.cpp
        src/session.cpp
        src/stdafx.cpp
        src/utils.cpp
    )

    target_link_libraries(join_server ${Boost_LIBRARIES})

    if(WIN32)
        add_definitions(
            -D_WIN32_WINDOWS
            -D_WINSOCK_DEPRECATED_NO_WARNINGS
        )
    endif()
endif()

target_include_directories(join_server
    PRIVATE "${CMAKE_BINARY_DIR}"
)

target_link_libraries(join_server PRIVATE Threads::Threads)

install(TARGETS join_server RUNTIME DESTINATION bin)
set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_FILE_NAME "join_server")
set(CPACK_PACKAGE_CONTACT art.shestakov@icloud.com)
include(CPack)
