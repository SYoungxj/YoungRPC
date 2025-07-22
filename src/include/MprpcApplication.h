#ifndef __MprpcApplication_H__
#define __MprpcApplication_H__

#include "NonCopyable.h"

class MprpcConfig;
/*
    框架的基础类--单例类
*/

class MprpcApplication : public NonCopyable
{
public:
    static auto GetInstance() -> MprpcApplication *;
    static void Init(int argc, char ** argv);    
    MprpcConfig * GetConfig();
private:
    MprpcApplication() = default;
    ~MprpcApplication() = default;
    static MprpcApplication * pInstance_;
    static MprpcConfig * pConfig_;    
};

#endif