cmake_minimum_required(VERSION 3.25)
project(Kolos4 C)

set(CMAKE_C_STANDARD 11)
set(THREADS_PREFER_PTHREAD_FLAG_ON)

find_package(Threads REQUIRED)

add_executable(Server server.c server.h utils.h)
add_executable(Client client.c client.h utils.h)

target_link_libraries(Server PRIVATE Threads::Threads ${CMAKE_THREAD_LIBS_INIT} rt)
target_link_libraries(Client PRIVATE Threads::Threads ${CMAKE_THREAD_LIBS_INIT} rt)
