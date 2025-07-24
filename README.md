# YoungRPC

## 使用说明

### 运行环境

Ubuntu 22.04 LTS

### 库准备

**第一步**：安装基础工具

```shell
sudo apt-get install -y wget cmake build-essential unzip
```

**第二步**：安装 Muduo（依赖boost） 

**Muduo **实质为: epoll + 线程池，优点是能够将网络I/O的代码和业务代码分开。而业务代码主要分为：用户的连接和断开、用户的可读写事件三类。至于什么时候发生这些事件，由网络库进行上报，如何监听这些事件，都是网络库所封装好的，我们就可以快速进行项目开发。

安装方式可参考：

[]: https://blog.csdn.net/QIANGWEIYUAN/article/details/89023980	"Muduo 库的安装"

**第三步**：安装 Protobuf

Protobuf 是一种二进制的格式，比使用 xml（20倍） 、json（10倍）进行数据交换快许多。可以把它用于分布式应用之间的数据通信或者异构环境下的数据交换。作为一种效率和兼容性都很优秀的二进制数据传输格式，可以用于诸如网络传输、配置文件、数据存储等诸多领域。  

```shell
sudo apt-get install protobuf-compiler libprotobuf-dev
```

**第四步**：安装 ZooKeeper

ZooKeeper 是分布式服务中的“指挥官”，在分布式环境中应用非常广泛，它的优秀功能很多，比如分布式环境中全局命名服务，服务注册中心，全局分布式锁等等。  

```shell
sudo apt install libzookeeper-mt-dev
```

### 编译指令

**方式一：**

```shell
cd YoungRPC
mkdir build
cd build
cmake ..
make
```

**方式二：**执行一键编译脚本

```shell
#1. 先为 autobuild.sh 增加可执行权限
chmod u+x autobuild.sh
#2. 一键执行
./autobuild.sh
```

### 测试用例

```shell
# 1. 进入 example 目录
cd example # 当前默认处在 YoungRPC 目录
# 2. 执行 protoc friend.proto --cpp_out=./
protoc friend.proto --cpp_out=./
# 3. 回到 build 目录重新编译
make # 当前处在 build 目录
#4. 进入 bin 目录运行测试程序
./provider -i test.conf
```

provider RPC 服务发布方启动显示如下

![image-20250724150338830](YoungRPC.assets/image-20250724150338830.png)

```shell
#5. 新开一个窗口运行 RPC 服务调用程序
./caller -i test.conf
```

caller 调用 Provider 中的 FriendService 服务的 GetFriendList() 函数，如下

![image-20250724151322912](YoungRPC.assets/image-20250724151322912.png)

### 使用 YoungRPC 帮助开发

```shell
# 假设项目名为 Project，copy 到 Project/third-party下
cp /你的路径/YoungRPC/lib/ -rf /你的路径/Project/third-pary
```

根据你的 CMakeLists.txt 自定义头文件搜索路径，target 链接选项等

注：链接需要带上如下库  YoungRPC protobuf muduo_net muduo_base pthread zookeeper_mt