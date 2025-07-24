#ifndef __MprpcApplication_H__
#define __MprpcApplication_H__

#include "NonCopyable.h"
#include "MprpcConfig.h"
#include "MprpcChannel.h"
#include "MprpcController.h"
#include "Logger.h"

/*
    框架的基础类--单例类
*/

class MprpcApplication : public NonCopyable
{
public:
    static auto GetInstance() -> MprpcApplication *;
    static void Init(int argc, char ** argv);    
private:
    class AutoRelease
    {
    public:
        AutoRelease() = default;
        ~AutoRelease()
        {
            if(nullptr != pInstance_)
            {
                delete pInstance_;
                pInstance_ = nullptr;
            }
        }
    };
    MprpcApplication() = default;
    ~MprpcApplication() = default;
    static MprpcApplication * pInstance_;    
};

#endif
