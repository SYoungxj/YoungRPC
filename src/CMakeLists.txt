# 查找 protobuf
find_package(Protobuf REQUIRED)

# 收集源文件
file(GLOB_RECURSE SRC_FILES "*.cpp" "*.cc")

# 创建静态库
add_library(YoungRPC STATIC ${SRC_FILES})

# 设置头文件搜索路径 - 这是关键！
target_include_directories(YoungRPC PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include    # 当前目录下的 include 文件夹
    ${PROJECT_SOURCE_DIR}/src/include      # 项目根目录下的 src/include
    ${Protobuf_INCLUDE_DIRS}
)

# 链接 protobuf
target_link_libraries(YoungRPC muduo_net muduo_base ${Protobuf_LIBRARIES} pthread zookeeper_mt)

# 设置库的输出位置
set_target_properties(YoungRPC PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
)