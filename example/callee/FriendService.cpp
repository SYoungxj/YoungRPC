#include "friend.pb.h"  
#include "MprpcApplication.h"
#include "RPCProvider.h"
#include "Logger.h"
#include <iostream>
#include <string>
#include <sys/types.h>
#include <vector>

using std::string;
using std::vector;

class FriendService : public Young::FriendServiceRPC  // 正确的类名
{
public:
    struct UserInfo
    {
        string name;
        uint32_t age;
        enum Gender
        {
            MALE = 0,
            FEMALE = 1
        };
        Gender gender;
        UserInfo(const string & name, uint32_t age, Gender gender)
        : name(name)
        , age(age)
        , gender(gender)
        {}
    };
    vector<UserInfo> GetFriendList(uint32_t userid)
    {
        vector<UserInfo> res;
        res.emplace_back("Kobe", 24, UserInfo::MALE);
        res.emplace_back("Irving", 11, UserInfo::MALE);
        res.emplace_back("Jordan", 23, UserInfo::FEMALE);
        cout << "This is local service GetFriendList\n";
        return res;
    }
    
    void GetFriendList(::google::protobuf::RpcController* controller,
                       const ::Young::GetFriendListRequest* request,
                       ::Young::GetFriendListResponse* response,
                       ::google::protobuf::Closure* done) override
    {
        // 获取 RPC 请求参数
        uint32_t userid = request->userid();
        
        // 执行本地函数
        for(auto & elem : GetFriendList(userid))
        {
            Young::User * user = response->add_friend_list();
            user->set_name(elem.name);
            user->set_age(elem.age);
            user->set_gender(static_cast<Young::User::Gender>(elem.gender));
        }
        
        // 执行回调
        // 序列化本地执行结果并通过网络发送返回
        done->Run();
    }
};

int main(int argc, char ** argv)
{
    LOG_INFO("This is First Log Message!");
    LOG_ERROR("%s:%s:%d", __FILE__, __FUNCTION__, __LINE__);

    MprpcApplication::Init(argc, argv);
    
    // 创建 RPC 发布方
    RPCProvider provider;
    provider.NotifyService(new FriendService());

    // 启动 RPC 服务发布节点
    // 阻塞等待 RPC 远程调用方发来的请求
    // 并响应
    provider.Run();
}
