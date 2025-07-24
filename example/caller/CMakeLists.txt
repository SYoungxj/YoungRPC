
# 设置源文件
# set(SRC_LIST CallUserService.cpp ../user.pb.cc)
set(SRC_LIST CallFriendService.cpp ../friend.pb.cc)

# 可用 GDB 调试
set(CMAKE_BUILD_TYPE Debug)

# 创建可执行文件
add_executable(caller ${SRC_LIST})

# 设置头文件搜索路径
target_include_directories(caller PUBLIC
    ${PROJECT_SOURCE_DIR}/src/include
    ${PROJECT_SOURCE_DIR}/example
)

# 链接库
target_link_libraries(caller
                        YoungRPC # 框架名
                        protobuf
                        ) 
