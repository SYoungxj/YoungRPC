
# 设置源文件
# set(SRC_LIST UserService.cpp ../user.pb.cc)

set(SRC_LIST FriendService.cpp ../friend.pb.cc)

# 可用 GDB 调试
set(CMAKE_BUILD_TYPE Debug)

# 创建可执行文件
add_executable(provider ${SRC_LIST})

# 设置头文件搜索路径
target_include_directories(provider PUBLIC
    ${PROJECT_SOURCE_DIR}/src/include
    ${PROJECT_SOURCE_DIR}/example
)

# 链接库
target_link_libraries(provider 
                        YoungRPC # 框架名
                        protobuf
                        ) 
