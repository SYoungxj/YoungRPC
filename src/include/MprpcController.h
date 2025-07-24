#ifndef __MprpcController_H__
#define __MprpcController_H__

#include <google/protobuf/service.h>
#include <string>

using std::string;

class MprpcController : public google::protobuf::RpcController
{
public:
    MprpcController();
    void Reset() override;
    bool Failed() const override;
    string ErrorText() const override;
    void SetFailed(const string & reason) override;


    // 目前未实现的功能
    void StartCancel() override;
    bool IsCanceled() const override;
    void NotifyOnCancel(google::protobuf::Closure * callback) override;

private:
    bool failed_; // RPC  方法执行过程中的状态
    string errorText; // RPC 方法执行过程中的错误信息
};

#endif
