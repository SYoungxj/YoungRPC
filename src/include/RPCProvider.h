#pragma once
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>

using std::cout;
using std::string;
using std::unordered_map;

/*
    框架提供的专门发布 RPC 服务的网络对象类
*/
class RPCProvider
{
public:
    // 这里是框架给外部使用的，可以发布 RPC 方法的函数接口
    void NotifyService(google::protobuf::Service * service);

    // 启动 RPC 服务节点，开始提供 RPC 远程网络调用服务
    void Run();

private:
    struct ServiceInfo
    {
        google::protobuf::Service * service_; // 保存服务对象
        // 方法信息
        unordered_map<string, const google::protobuf::MethodDescriptor*> methodMap_; 
    };

    // 存储注册成功的服务对象和其服务方法的所有信息
    unordered_map<string, ServiceInfo> serviceMap_;

    // 组合 EventLoop
    muduo::net::EventLoop eventLoop_;
    
    // 新的连接回调函数
    void OnConnection(const muduo::net::TcpConnectionPtr&);

    // 已建立连接用户的读写事件回调函数
    void OnMessage(const muduo::net::TcpConnectionPtr & conn, muduo::net::Buffer * buffer, muduo::Timestamp timestamp);

    // Closure 的回调操作，用于序列化 RPC 的响应和网络发送
    void SendResponse(const muduo::net::TcpConnectionPtr &, google::protobuf::Message*);
};
