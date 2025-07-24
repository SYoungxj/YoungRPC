#include "user.pb.h"  
#include "MprpcApplication.h"
#include "RPCProvider.h"
#include <iostream>
#include <string>
#include <sys/types.h>

using std::cout;
using std::string;

class UserService : public Young::UserServiceRPC  // 正确的类名
{
public:
    bool Login(string name, string pwd)
    {
        cout << "This is local service Login\n";
        cout << "name: " << name << " pwd: " << pwd << "\n";
        return 1;  // 添加了返回值
    }

    /*
        重写基类 UserServiceRPC 的虚函数
        下面这些方法都是 框架 直接调用的
        1. caller ===> Login(LoginRequest) => muduo => callee
        2. callee ===> Login(LoginRequest) => 交给下面重写的 Login 方法上了
    */
    void Login(::google::protobuf::RpcController* controller,
                       const ::Young::LoginRequest* request,
                       ::Young::LoginResponse* response,
                       ::google::protobuf::Closure* done) override
    {
        // 框架给业务上报了请求参数 LoginRequest，应用获取相应数据做本地业务
        string name = request->name();
        string pwd = request->pwd();

        // 做本地业务
        if(Login(name, pwd))
        {
            // 把响应写入
            // 包括：错误码，错误信息，返回值
            Young::ResultCode * errcode = response->mutable_result();
            errcode->set_errcode(0);
            errcode->set_errmsg("Login Success");
            response->set_success(1);
        }
        else
        {
            Young::ResultCode * errcode = response->mutable_result();
            errcode->set_errcode(1);
            errcode->set_errmsg("Login Failed");
            response->set_success(0);
        }
        
        // 执行回调操作:
        // 执行响应对象数据的序列化和网络发送（都是由框架完成）
        done->Run();
    }

    bool Register(uint32_t id, string name, string pwd)
    {
        cout << "This is local service Register\n";
        cout << "id: " << id << " name: " << name << " pwd: " << pwd << "\n";
        return true;  // 添加了返回值
    }

    void Register(::google::protobuf::RpcController* controller,
                       const ::Young::RegisterRequest* request,
                       ::Young::RegisterResponse* response,
                       ::google::protobuf::Closure* done) override
    {
        // 获取参数
        uint32_t id = request->id();
        string name = request->name();
        string pwd = request->pwd();
        
        // 做本地业务
        // 并且填充响应的错误码信息
        if(Register(id, name, pwd))
        {
            response->mutable_result()->set_errcode(0);
            response->mutable_result()->set_errmsg("SUCCESS");
            response->set_success(true);
        }
        else 
        {
            response->mutable_result()->set_errcode(1);
            response->mutable_result()->set_errmsg("FAILED");
            response->set_success(false);    
        }

        // 执行回调
        // 自定义的 Closure * callback
        // 主要负责：1. 序列化  2. 通过网络发送给 PRC 调用方 
        done->Run();
    }

};

int main(int argc, char ** argv)
{
    // 调用框架的初始化操作
    MprpcApplication::Init(argc, argv);

    // provider 是一个 RPC 网络服务对象
    // 把 UserService 对象发布到 RPC 节点上
    RPCProvider provider;
    provider.NotifyService(new UserService());
    
    // 启动一个 RPC 服务发布节点
    // RUN() 以后进程进入阻塞状态
    // 等待远程的 RPC 调用请求
    provider.Run();

    return 0;
}
