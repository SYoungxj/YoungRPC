#include "MprpcController.h"
#include <iostream>

using std::cout;

MprpcController::MprpcController()
: failed_(false)
, errorText("")
{

}

void MprpcController::Reset()
{
    // 重置信息
    failed_ = false;
    errorText = "";
}

bool MprpcController::Failed() const
{
    return failed_;
}

string MprpcController::ErrorText() const
{
    return errorText;
}

void MprpcController::SetFailed(const string & reason)
{
    failed_ = true;
    errorText = reason;
}


    // 目前未实现的功能
void MprpcController::StartCancel()
{

}

bool MprpcController::IsCanceled() const
{
    return false;
}

void MprpcController::NotifyOnCancel(google::protobuf::Closure * callback)
{

}
