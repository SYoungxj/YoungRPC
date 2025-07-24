#include "ZookeeperUtil.h"
#include "MprpcApplication.h"
#include <semaphore.h>
#include <iostream>

using std::cout;

// 全局的 watcher 观察器
void global_watcher(zhandle_t * zhandle, int type, int state,
                     const char *path, void * watcherCtx)
{
    if(type == ZOO_SESSION_EVENT) // 回调的消息类型和 Session 相关
    {
        if(state == ZOO_CONNECTED_STATE) // zkclient 和 zkserver 连接成功
        {
            sem_t * sem = (sem_t*)watcherCtx;
            sem_post(sem); // 信号量资源 + 1
        }
    }
}


ZkClient::ZkClient()
: zhandle_(nullptr)
{

}

ZkClient::~ZkClient()
{
    // 关闭句柄释放资源
    if(nullptr != zhandle_)
    {
        zookeeper_close(zhandle_);
    }
}

// zkclient 启动连接 zkserver
// 异步启动 zookeeper 服务
void ZkClient::Start()
{
    string host = MprpcConfig::GetInstance()->Load("ZookeeperIP");
    string port = MprpcConfig::GetInstance()->Load("ZookeeperPORT");
    string connAddr = host + ":" + port;

    /*
        zookeeper_mt 多线程版本
        zookeeper 的 API 客户端程序提供了 3 个线程
        API 调用线程
        网络 I/O 线程 pthread_create 和 poll(无需高并发)
        watcher 回调线程
    */
    // 为 zookeeper 创建内存资源
    zhandle_ = zookeeper_init(connAddr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
    if(nullptr == zhandle_)
    {
        LOG_ERROR("zookeeper_init error!");
        exit(EXIT_FAILURE);
    }

    // 创建信号量，等待唤醒
    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(zhandle_, &sem); // 绑定上下文

    sem_wait(&sem);
    
    cout << "zookeeper_init success!!!\n";
}
    
// 在 zkserver 上根据指定的 path 创建 znode 节点
void ZkClient::Create(const char * path, const char * data, int dataLen, int state)
{
    char path_buffer[128];
    int bufferlen = sizeof(path_buffer);
    int flag;

    // 先检查节点是否已经存在
    flag = zoo_exists(zhandle_, path, 0, nullptr);
    if(flag == ZOK)
    {
        cout << "znode already exists... path: " << path << "\n";
        return; // 节点已存在，直接返回
    }
    
    // 节点不存在，创建新节点
    flag = zoo_create(zhandle_, path, data, dataLen, &ZOO_OPEN_ACL_UNSAFE, state, path_buffer, bufferlen);
    if(flag == ZOK)
    {
        cout << "znode create success... path: " << path << "\n";
    }
    else
    {
        cout << "znode create error... path: " << path << " flag: " << flag << "\n";
        exit(EXIT_FAILURE);
    }
}

// 根据参数指定的 znode 节点路径，获取 znode 节点的值
string ZkClient::GetData(const char * path)
{
    char buffer[64];
    int bufferlen = sizeof(buffer);
    
    int flag = zoo_get(zhandle_, path, 0, buffer, &bufferlen, nullptr);
    if(flag != ZOK)
    {
        cout << "get znode failed... path: "  << path << "\n";
        return "";
    }
    else
    {
        return buffer;
    }
}
