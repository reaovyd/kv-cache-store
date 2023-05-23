cmake_minimum_required(VERSION 3.20)

set(EXEC "kv-cache-store")
project(${EXEC} VERSION 0.0.1 HOMEPAGE_URL "https://github.com/reaovyd/kv-cache-store" LANGUAGES C)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

find_package(Threads REQUIRED)
find_package(Libevent REQUIRED)

set(SRCF "${CMAKE_SOURCE_DIR}/src")
set(INCF "${CMAKE_SOURCE_DIR}/include")
set(TSTF "${CMAKE_SOURCE_DIR}/tests")

set(EXEC_TESTS "${EXEC}_tests")
set(EXEC_DEBUG "${EXEC}_debug")

set(MAIN "${SRCF}/main.c")

### shared flags ####
set(WFLAGS "-Wall -Werror -Wextra -Wpedantic -Wno-unused-function")
set(PTHREAD_LIB "pthread")
set(LIBEVENT_LIB "event")

### test lib flag thread ###
set(CRITERION_LIB "criterion")



### debug flag ###
set(DEBUG_FLAG "-g")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WFLAGS}")

# getting all files in respective directory
set(ALL_SRCF "${SRCF}/server.c")
file(GLOB ALL_INCF "${INCF}/*.h")
file(GLOB ALL_TSTF "${TSTF}/*.c")

add_executable(${EXEC} ${MAIN} ${ALL_SRCF} ${ALL_INCF})
add_executable(${EXEC_TESTS} ${ALL_TSTF} ${ALL_SRCF} ${ALL_INCF})

target_include_directories(${EXEC}       PRIVATE ${INCF})
target_include_directories(${EXEC_TESTS} PRIVATE ${INCF})

target_link_libraries(${EXEC} ${PTHREAD_LIB} ${LIBEVENT_LIB})
target_link_libraries(${EXEC_TESTS} ${PTHREAD_LIB} ${LIBEVENT_LIB} ${CRITERION_LIB})

# debug = development
if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    add_executable(${EXEC_DEBUG} ${MAIN} ${ALL_SRCF} ${ALL_INCF})
    target_include_directories(${EXEC_DEBUG} PRIVATE ${INCF})
    target_compile_options(${EXEC_DEBUG} PRIVATE ${DEBUG_FLAG})
    target_link_libraries(${EXEC_DEBUG} ${PTHREAD_LIB} ${LIBEVENT_LIB})
endif()
