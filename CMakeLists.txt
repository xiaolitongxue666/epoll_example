cmake_minimum_required(VERSION 3.19)
project(epoll_example C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(epoll_example
        epoll_example.c
        epoll_example.h)
