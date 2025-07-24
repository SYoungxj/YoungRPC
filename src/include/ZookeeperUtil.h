#ifndef __ZookeeperUtil_H__
#define __ZookeeperUtil_H__

#include <semaphore.h>
#include <zookeeper/zookeeper.h>
#include <string>

using std::string;

// 封装的 zk 客户端
class ZkClient
{
public:
    ZkClient();
    ~ZkClient();

    // zkclient 启动连接 zkserver
    void Start();
    
    // 在 zkserver 上根据指定的 path 创建 znode 节点
    void Create(const char * path, const char * data, int dataLen, int state = 0);

    // 根据参数指定的 znode 节点路径，获取 znode 节点的值
    string GetData(const char * path);

private:
    // zk 的客户端句柄，类似于 MySQL 
    zhandle_t * zhandle_;
};





#endif
