#include "test.pb.h"
#include <iostream>
#include <string>

using std::cout;
using std::string;
using namespace Young;

void test0()
{
    // 设置请求
    LoginRequest req;
    req.set_name("Young");
    req.set_pwd("123");
    
    // 序列化请求
    string send_str;
    if(req.SerializeToString(&send_str))
    {
        cout << send_str << "\n";
    }
    
    // 反序列化
    LoginRequest req2;
    if(req2.ParseFromString(send_str))
    {
        cout << req2.name() << "\n";
        cout << req2.pwd() << "\n";
    }
}

void test1()
{
    LoginResponse resp1;
    ResultCode * rc1 = resp1.mutable_result();
    rc1->set_errcode(0);
    rc1->set_errmsg("SUCCESS");
    
    GetFriendListsResponse resp2;
    ResultCode * rc2 = resp2.mutable_result();
    rc2->set_errcode(1);
    rc2->set_errmsg("FAILED");

    User * user1 = resp2.add_friend_list();
    user1->set_name("Young");
    user1->set_age(18);
    user1->set_gender(User::MALE);

    cout << resp2.friend_list_size() << "\n";

}

int main()
{
    test1();
    return 0;
}
