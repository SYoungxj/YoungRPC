#ifndef __MprpcChannel_H__
#define __MprpcChannel_H__

#include <google/protobuf/service.h>

class MprpcChannel : public google::protobuf::RpcChannel
{
public:
    void CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                          google::protobuf::Message* response, google::protobuf::Closure* done) override;
};

#endif
