
# 设置源文件
set(SRC_LIST UserService.cpp ../user.pb.cc)

# 创建可执行文件
add_executable(provider ${SRC_LIST})

# 设置头文件搜索路径
target_include_directories(provider PUBLIC
    ${PROJECT_SOURCE_DIR}/src/include
    ${PROJECT_SOURCE_DIR}/example
)

# 链接库
target_link_libraries(provider 
                        mpRPC # 框架名
                        protobuf
                        ) 
