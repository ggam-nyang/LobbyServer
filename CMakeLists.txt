cmake_minimum_required(VERSION 3.26)
project(LobbyServer)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost
        COMPONENTS
        thread
        chrono
        REQUIRED
)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    #    link_directories(${Boost_LIBRARY_DIRS})
    #    add_definitions( "-DHAS_BOOST" )
    #    Message(STATUS "Boost found")
endif ()

add_executable(LobbyServer
        server/serverMain.cpp
        server/Server.cpp
        server/Server.h
        server/Session.cpp
        server/Protocol.cpp
        server/Protocol.h
)

add_executable(LobbyClient
        client/clientMain.cpp
        client/Client.cpp
        client/Client.h
        server/Protocol.cpp
        server/Protocol.h
)

target_link_libraries(LobbyServer ${Boost_LIBRARIES})
target_link_libraries(LobbyClient ${Boost_LIBRARIES})
