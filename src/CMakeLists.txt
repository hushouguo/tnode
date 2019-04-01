set(target tnode) 
project (${target})

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_BUILD_TYPE "Debug")#Release

set(PROJECT_DIR .)
set(EXECUTABLE_OUTPUT_PATH ..)

add_definitions(-Wall -Werror -D_GNU_SOURCE -DLINUX -DX64 -fstack-protector-all -D_LARGEFILE64_SOURCE -DDEBUG -std=c++11)

include_directories(.)
include_directories(${DEPS_DIR})
include_directories(${PROTOBUF_DIR}/include)
include_directories(${TCMALLOC_DIR}/include)
include_directories(${MYSQL_DIR}/include)
include_directories(${RAPIDXML_DIR})
include_directories(${RAPIDJSON_DIR})
include_directories(${LIBEVENT_DIR}/include)
include_directories(${CURL_DIR}/include)
include_directories(${LUA_DIR}/include)
#include_directories(${LIBUV_DIR}/include)
#include_directories(${LIBZMQ_DIR}/include)

aux_source_directory(config src)
#aux_source_directory(db src)
aux_source_directory(logger src)
aux_source_directory(json src)
aux_source_directory(lua src)
#aux_source_directory(net src)
#aux_source_directory(recordclient src)
#aux_source_directory(sha src)
#aux_source_directory(socket src)
aux_source_directory(time src)
aux_source_directory(tools src)
aux_source_directory(xml src)
aux_source_directory(service src)

link_directories(/usr/lib64)
add_executable(${target} ${src})

target_link_libraries(${target} rt dl pthread unwind crypto ssl)
target_link_libraries(${target} ${LIBEVENT_DIR}/lib/libevent.a)
target_link_libraries(${target} ${LUA_DIR}/lib/libluajit-5.1.a)
target_link_libraries(${target} ${PROTOBUF_DIR}/lib/libprotobuf.a)
target_link_libraries(${target} ${MYSQL_DIR}/lib/libmysqlclient.a)
target_link_libraries(${target} ${CURL_DIR}/lib/libcurl.a)
target_link_libraries(${target} ${TCMALLOC_DIR}/lib/libtcmalloc_minimal.a) 

