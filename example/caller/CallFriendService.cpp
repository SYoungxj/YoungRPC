#include "MprpcApplication.h"
#include "friend.pb.h"
#include <iostream>

using std::cout;

int main(int argc, char ** argv)
{
    // 初始化框架
    MprpcApplication::Init(argc, argv);
    
    // 新建 RPC stub
    Young::FriendServiceRPC_Stub stub(new MprpcChannel());

    // 准备 RPC 方法的请求
    Young::GetFriendListRequest req;
    req.set_userid(8);

    // 准备 RPC 方法的响应
    Young::GetFriendListResponse resp;

    // 创建一个 RPC 控制类
    MprpcController controller;

    // 准备 RPC 方法的响应并传入 stub 调用的方法中
    stub.GetFriendList(&controller, &req, &resp, nullptr);
    
    if(controller.Failed())
    {
        cout << controller.ErrorText() << "\n";
    }
    else
    {
        if(0 == resp.result().errcode())
        {
            cout << "RPC GetFriendList Response Success\nThe friend lists are as follows:\n";
            for(int i = 0; i < resp.friend_list_size(); ++i)
            {
                const Young::User user = resp.friend_list(i);
                cout << user.name() << "-" << user.age() << "-" << user.gender() << "\n";
            }
        }
        else
        {
            cout << "RPC GetFriendList Response Failed: " << resp.result().errmsg() << "\n";
        }
    }

    return 0;

}
