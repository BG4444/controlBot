cmake_minimum_required(VERSION 2.8)

project(controlBot)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Werror")

find_package(PkgConfig REQUIRED)
find_package(gflags REQUIRED)
find_package(CURL REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(Threads REQUIRED)
find_library(GCRYPT_LIBRARY NAMES gcrypt libgcrypt)
find_package(ICU 52.0 COMPONENTS uc  REQUIRED)
find_package(folly REQUIRED)


pkg_check_modules(JSONCPP QUIET jsoncpp)

if(JSONCPP_LIBRARIES)
        message(STATUS "Found JsonCpp: ${JSONCPP_LIBRARIES}")
        if(NOT JSONCPP_INCLUDE_DIRS STREQUAL "")
                message(STATUS "Found JsonCpp headers: ${JSONCPP_INCLUDE_DIRS}")
                include_directories("${JSONCPP_INCLUDE_DIRS}")
        endif()
else()
        message(FATAL_ERROR "!! Cannot find JsonCpp")
endif()


add_executable(${PROJECT_NAME} "main.cpp"  "botconnection.cpp" "user.cpp" "dbwrapper.cpp" "opt-parser/optparser.cpp" "opt-parser/netportnumber.cpp")


target_link_libraries(${PROJECT_NAME}
                      ${CMAKE_SOURCE_DIR}/xxtelebot/build/lib/libxxtelebot.a
                     )
target_link_libraries(${PROJECT_NAME}
        PRIVATE
                ${CURL_LIBRARIES}
                ${JSONCPP_LIBRARIES}
                ${GFLAGS_LIBRARIES}
                ${FOLLY_LIBRARIES}
                pq
                ${GCRYPT_LIBRARY}
                Threads::Threads)

target_include_directories(${PROJECT_NAME}
        PRIVATE
                ${PostgreSQL_INCLUDE_DIRS}
                ${FOLLY_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${FOLLY_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${ICU_LIBRARIES})
