cmake_minimum_required(VERSION 3.15)
project(bitwise_test C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

add_executable(main main.c queue.h queue.c)
add_executable(producent1 produc1.c queue.h queue.c )
add_executable(producent2 produc2.c queue.h queue.c )
add_executable(producent3 produc3.c queue.h queue.c )
add_executable(konsument1 kons1.c queue.h queue.c )
add_executable(konsument2 kons2.c queue.h queue.c )
add_executable(konsument3 kons3.c queue.h queue.c )
add_executable(producentSpec producSpec.c queue.h queue.c )
