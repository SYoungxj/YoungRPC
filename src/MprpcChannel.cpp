#include "MprpcConfig.h"
#include "MprpcChannel.h"
#include "MprpcController.h"
#include "RPCHeader.pb.h"
#include "ZookeeperUtil.h"
#include "Logger.h"
#include <google/protobuf/descriptor.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>

using std::cout;
using std::string;

/*
    HEADER: header_size + service_name + method_name + args_size 

 */
// 所有通过 Stub 代理对象调用的 RPC 方法都在这里
// 统一做 RPC 方法调用的数据序列化和网络发送
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                          google::protobuf::Message* response, google::protobuf::Closure* done)
{
    const google::protobuf::ServiceDescriptor * serviceDesc = method->service();

    string serviceName = serviceDesc->name();
    string methodName = method->name();

    // 获取参数的序列化字符串长度 args_size
    string args_str;
    uint32_t args_size;
    if(request->SerializeToString(&args_str))
    {
        args_size = args_str.size();
    }
    else
    {
        // 填充控制信息
        controller->SetFailed("Serialize Request Arguments Error!");
        LOG_ERROR("Serialize Request Arguments Error!");
        return;
    }

    // 定义 RPC 请求的 HEADER
    mprpc::RPCHeader RPC_header;
    RPC_header.set_service_name(serviceName);
    RPC_header.set_method_name(methodName);
    RPC_header.set_args_size(args_size);
    
    string RPC_header_str;
    uint32_t header_size;
    // if(request->SerializeToString(&RPC_header_str)) 错误
    if(RPC_header.SerializeToString(&RPC_header_str))
    {
        header_size = RPC_header_str.size();        
    }
    else
    {
        controller->SetFailed("Serialize Request Header Error!");
        LOG_ERROR("Serialize Request Header Error!");
        return;
    }

    // 组织待发送的 RPC 请求的字符串
    string send_RPC_str;
    // 插入 header_size
    send_RPC_str.insert(0, string((char*)&header_size, 4));
    send_RPC_str += RPC_header_str;
    send_RPC_str += args_str;

    // 打印调试信息
    cout << "=====================================\n"; 
    cout << "service_name: " << serviceName << "\n";
    cout << "method_name: " << methodName << "\n";
    cout << "args_size: " << args_size << "\n";
    cout << "args_str (hex): ";
    for (char c : args_str) {
        printf("%02x ", (unsigned char)c);
    }
    cout << "\n";
    cout << "header_size: " << header_size << "\n";
    cout << "RPC_header_str (hex): ";
    for (char c : RPC_header_str) {
        printf("%02x ", (unsigned char)c);
    }
    cout << "\n";
    cout << "=====================================\n";

    // TCP 编程
    int clientFd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == clientFd)
    {
        char error_buf[512] = {0};
        sprintf(error_buf, "Create Socket Error: %d", errno);
        controller->SetFailed(error_buf);
        LOG_ERROR("%s", error_buf);
        return;
    }

    
    /* // 获取 RPC Provider 的IP 和 PORT */
    /* string ip = MprpcConfig::GetInstance()->Load("RPCServerIP"); */
    /* uint16_t port = atoi(MprpcConfig::GetInstance()->Load("RPCServerPORT").c_str()); */

    // 不用和 RPC Provider 直接交互
    // 通过分布式服务协调 Zookeeper 查询 service:method 的 ip:port 
    ZkClient zkCli;
    zkCli.Start();
    
    string method_path = "/" + serviceName + "/" + methodName;
    string host_data = zkCli.GetData(method_path.c_str());
    if(host_data == "")
    {
        string error_msg = method_path + "is not exist";
        controller->SetFailed(error_msg.c_str());
        LOG_ERROR("%s", error_msg.c_str());
        return;
    }

    int idx = host_data.find(":");
    if(idx == -1)
    {
        string error_msg = method_path + "address is invalid";
        controller->SetFailed(error_msg.c_str());
        LOG_ERROR("%s", error_msg.c_str());
        return;
    }

    string ip = host_data.substr(0, idx);
    uint16_t port = atoi(host_data.substr(idx+1, host_data.size()-idx).c_str());

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

    if(-1 == connect(clientFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)))
    {
        char error_buf[512] = {0};
        sprintf(error_buf, "Connection to RPC Provider Error: %d", errno);
        controller->SetFailed(error_buf);
        LOG_ERROR("%s", error_buf);
        close(clientFd);
        return;
    }

    if(-1 == send(clientFd, send_RPC_str.c_str(), send_RPC_str.size(), 0))
    {
        char error_buf[512] = {0};
        sprintf(error_buf, "Send to PRC Provider Error: %d", errno);
        controller->SetFailed(error_buf);
        LOG_ERROR("%s", error_buf);
        close(clientFd);
        return;
    }


    // 接收 RPC 请求的响应值
    char recv_buf[1024] = {0};
    int recv_size = 0;
    if(-1 == (recv_size = recv(clientFd, recv_buf, 1024, 0)))
    {
        char error_buf[512] = {0};
        sprintf(error_buf, "Recv From PRC Provider Error: %d", errno);
        controller->SetFailed(error_buf);
        LOG_ERROR("%s", error_buf);
        close(clientFd);
        return;
    }

    // 反序列化 RPC 调用的响应数据
    // string resp_str(recv_buf, 0, recv_size); 字符串构造时遇到 \0 会截断 出现问题
    // if(!response->ParseFromString(resp_str))
    if(!response->ParseFromArray(recv_buf, recv_size))
    {
        char error_buf[512] = {0};
        sprintf(error_buf, "PRC Response Parse Error: %d", errno);
        controller->SetFailed(error_buf);
        LOG_ERROR("%s", error_buf);
        close(clientFd);
        return;
    }

    close(clientFd);
}
