cmake_minimum_required(VERSION 3.20)

add_executable(simple-test main.cpp)

install(TARGETS simple-test
    RUNTIME DESTINATION bin
)
