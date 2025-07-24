#include "MprpcApplication.h"
#include "user.pb.h"
#include <iostream>

using std::cout;

int main(int argc, char ** argv)
{
    // 整个程序启动后，想使用 RPC 框架服务调用，一定要先调用
    // 框架的初始化函数
    MprpcApplication::Init(argc, argv); 

    // 演示调用远程发布的 RPC 方法 Login
    Young::UserServiceRPC_Stub stub(new MprpcChannel());

    // 准备 RPC 方法的请求
    Young::LoginRequest loginReq;
    loginReq.set_name("Young");
    loginReq.set_pwd("123");

    // 准备 RPC 方法的响应
    Young::LoginResponse loginResp;

    // 本质上是 RPCChannel->CallMethod()
    stub.Login(nullptr, &loginReq, &loginResp, nullptr);

    // 一次 RPC 调用完成以后
    // 获取调用结果
    if(0 == loginResp.result().errcode())
    {
        cout << "RPC Login Response Success: " << loginResp.success() << "\n";
    }
    else
    {
        cout << "RPC Login Response Fail: " << loginResp.result().errmsg() << "\n";
    }

    // 请求调用 Register PRC 服务
    // 填写远程 RPC 请求参数
    Young::RegisterRequest registerReq;
    registerReq.set_id(8);
    registerReq.set_name("Xujun");
    registerReq.set_pwd("456");

    Young::RegisterResponse registerResp;

    // 通过 Stub 调用远程 RPC Register 服务
    stub.Register(nullptr, &registerReq, &registerResp, nullptr);
    
    // 一次 RPC 调用完成以后
    // 获取调用结果
    if(0 == registerResp.result().errcode())
    {
        cout << "RPC Register Response Success: " << registerResp.success() << "\n";
    }
    else
    {
        cout << "RPC Register Response Fail: " << registerResp.result().errmsg() << "\n";
    }

    return 0;
}
