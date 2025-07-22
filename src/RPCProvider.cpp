#include "RPCProvider.h"
#include "MprpcApplication.h"
#include "MprpcConfig.h"
#include "RPCHeader.pb.h"

/*
    service name => service descriptor
    => => service * 记录服务对象
    => => method name => method 方法对象
 */
// 这里是框架提供给外部使用的，可以发布 RPC 方法的函数接口
void RPCProvider::NotifyService(google::protobuf::Service * service)
{
    ServiceInfo serviceInfo;

    // 获取了服务对象的描述信息
    const google::protobuf::ServiceDescriptor * pServiceDesc = service->GetDescriptor();
    
    // 获取服务的名字
    string serviceName = pServiceDesc->name();
    
    // 获取服务对象 service 方法的数量
    int methodNum = pServiceDesc->method_count();

    for(int i = 0; i < methodNum; ++i)
    {
        // 获取服务对象指定下标的方法的描述（抽象描述）
        const google::protobuf::MethodDescriptor * pMethodDesc = pServiceDesc->method(i);
        string methodName = pMethodDesc->name();
        // 存储对应服务对象的方法
        serviceInfo.methodMap_.insert({methodName, pMethodDesc});
        cout << methodName << "\n";
    }
    
    cout << serviceName << "\n";
    // 存储该服务对象
    serviceMap_.insert({serviceName, serviceInfo});
}

void RPCProvider::Run()
{
    // 从配置类读取 RPC 的网络 IP 和 PORT
    string ip = MprpcApplication::GetInstance()->GetConfig()->Load("RPCServerIP");
    uint16_t port = atoi(MprpcApplication::GetInstance()->GetConfig()->Load("RPCServerPORT").c_str());
    muduo::net::InetAddress address(ip, port);

    // 创建 TcpServer 对象
    muduo::net::TcpServer server(&eventLoop_, address, "RPCProvider");
    
    // 绑定连接回调
    using namespace std::placeholders;
    server.setConnectionCallback(std::bind(&RPCProvider::OnConnection, this, _1));
    // 绑定消息回调
    server.setMessageCallback(std::bind(&RPCProvider::OnMessage, this, _1, _2, _3));

    // 设置 muduo 库的线程数量
    server.setThreadNum(4);

    // 启动服务器并监听
    server.start();
    eventLoop_.loop();
}

// 连接回调函数
void RPCProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn)
{
    if(!conn->connected())
    {
        // 和 RPC Client 连接断开
        conn->shutdown();
    }
}

/*
    在框架内部，RPCProvider 和 RPCConsumer 需要协商好通信用的 protobuf 数据类型
    service_name method_name args   定义 protobuf 的message 类型
    进行数据头的序列化和反序列化
    错误示例：UserServiceLoginzhang 123456
    正确示例：header_size UserServiceName MethodName args_size
                  24        LoginService    Login      11
 */

// 已建立连接用户的读写事件回调，如果远程有一个 RPC 服务的调用请求
// OnMessage 方法就会响应
void RPCProvider::OnMessage(const muduo::net::TcpConnectionPtr & conn, muduo::net::Buffer * buffer, muduo::Timestamp timestamp)
{
    // 接受的数据是远程 PRC 调用请求的字节流
    string recv_buf = buffer->retrieveAllAsString();

    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4, 0); // 获取前 4 个字节
    
    // 获取header
    string RPC_header_str = recv_buf.substr(4, header_size);

    mprpc::RPCHeader RPC_header;
    string serviceName;
    string methodName;
    uint32_t args_size;
    if(RPC_header.ParseFromString(RPC_header_str))
    {
        // 数据头反序列化成功 
        serviceName = RPC_header.service_name();
        methodName = RPC_header.method_name();
        args_size = RPC_header.args_size();
    }
    else
    {
        // 后续用日志代替打印信息
        cout << "RPC_header_str: " << RPC_header_str << "parse failed\n";
    }

    // 获取 RPC 方法参数的字符流数据
    string args_str = recv_buf.substr(4 + header_size, args_size);

    // 打印调试信息
    cout << " =====================================\n"; 
    cout << "header_size: " << header_size << "\n";
    cout << "RPC_header_str: " << RPC_header_str << "\n";
    cout << "service_name: " << serviceName << "\n";
    cout << "method_name: " << methodName << "\n";
    cout << "args_str: " << args_str << "\n";
    cout << " =====================================\n"; 
    
    // 获取 service 对象和 method 对象
    auto it = serviceMap_.find(serviceName);
    if(it == serviceMap_.end())
    {
        cout << serviceName << "is not exist\n";
        return;
    }
    
    auto it1 = it->second.methodMap_.find(methodName);
    if(it1 == it->second.methodMap_.end())
    {
        cout << methodName << "is not exist\n";
        return;
    }

    // 获取 Service 对象
    google::protobuf::Service * service = it->second.service_;

    // 获取 Methode 对象
    const google::protobuf::MethodDescriptor * method = it1->second;

    // 获取 method_request 和method_response
    google::protobuf::Message * req = service->GetRequestPrototype(method).New();
    if(!req->ParseFromString(args_str))
    {
        cout << "request parse failed! content: " << args_str << "\n"; 
    }
    google::protobuf::Message * resp = service->GetResponsePrototype(method).New();

    // 创建 Closure 回调函数：
    // 进行 response 的序列化并通过网络发送
    /* google::protobuf::Closure * done = google::protobuf::NewCallback(this, &RPCProvider::SendResponse, conn, resp); */
    google::protobuf::Closure * done = google::protobuf::NewCallback // protobuf 的 NewCallback 函数
                                        <RPCProvider, const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>
                                        (this,
                                        &RPCProvider::SendResponse, 
                                        conn, 
                                        resp);

    // 在框架上根据远端 RPC 请求，调用当前 RPC 节点上发布的方法
    service->CallMethod(method, nullptr, req, resp, done);
}

void RPCProvider::SendResponse(const muduo::net::TcpConnectionPtr & conn, google::protobuf::Message* resp)
{
    // 1. 将 response 反序列化
    string resp_str;
    if(!resp->SerializeToString(&resp_str))
    {
        // 序列化成功后，通过 conn 发送回 RPC 调用方
        conn->send(resp_str);
    }
    else
    {
        cout << "response serializae failed!\n";
    }
    conn->shutdown(); // 模拟 HTTP 的短连接服务，发送完以后由 RPCProvider 断开连接
}
