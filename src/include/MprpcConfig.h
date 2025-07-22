#ifndef __MprpcConfig_H__
#define __MprpcConfig_H__

#include "NonCopyable.h"
#include <unordered_map>
#include <string>

using std::unordered_map;
using std::string;

/*
    框架读取配置文件的类
*/
class MprpcConfig : public NonCopyable
{
public:
    static auto GetInstance() -> MprpcConfig *;
    // 负责解析加载配置文件
    void LoadConfigFile(const string & configFile);

    // 查询配置项信息
    auto Load(const string & key) -> string;

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
    MprpcConfig() = default;
    ~MprpcConfig() = default;

    unordered_map<string, string> configMap_;
    static MprpcConfig * pInstance_;
};

#endif
